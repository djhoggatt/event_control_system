"""
Author: Denver Hoggatt
Description: Invoke tasks

Copyright (c) 2025 Denver Hoggatt. All rights reserved.

This software is licensed under the terms stated in the LICENSE file
located at the root of this repository. If no LICENSE file accompanies
this software, it is provided "AS IS" WITHOUT WARRANTY OF ANY KIND.
"""

# --------------------------------------------------------------------------------------------------
#  Imports
# --------------------------------------------------------------------------------------------------

from invoke import task
from invoke import UnexpectedExit
from os import getcwd
from pathlib import Path
from platform import system
from time import sleep
from subprocess import run

# --------------------------------------------------------------------------------------------------
# Global Constants
# --------------------------------------------------------------------------------------------------


# --------------------------------------------------------------------------------------------------
# Global Variables
# --------------------------------------------------------------------------------------------------


# --------------------------------------------------------------------------------------------------
# Classes
# --------------------------------------------------------------------------------------------------


# --------------------------------------------------------------------------------------------------
# Functions
# --------------------------------------------------------------------------------------------------


def _get_vars(c):
    """
    Gets variables common to various common to tasks.
    Return: list of common variables.
    """

    vars = {}

    vars["build_dir"] = str(Path(getcwd()) / Path("build/"))

    vars["jlink"] = Path("JLinkExe")
    vars["jlink_gdb"] = Path("JLinkGDBServerCLExe")

    vars["doxy"] = "doxygen"

    try:
        with open(Path(vars["build_dir"]) / Path("last_build.txt"), "r") as file:
            vars["last_built"] = file.read().strip()
    except FileNotFoundError:
        vars["last_built"] = None

    return vars


def _subproject_deps(c, verbose):
    """
    Sets up meson subproject dependencies
    """

    try:
        c.run("mkdir -p subprojects", echo=verbose)
        c.run("meson wrap install gtest", echo=verbose)
    except UnexpectedExit:
        pass  # Already installed


def _generate_code(c, vars, verbose):
    """
    Creates any auto-generated code files used for the build.
    """

    platform_path = Path("platforms") / Path(vars["platform"])
    build_path = str(Path(vars["build_dir"]) / Path(vars["build_name"]))
    inc_path = Path(build_path) / Path("include")
    io_toml = str(platform_path / Path("io.toml"))
    c.run(
        f"python scripts/generate_io.py --io {io_toml} --dir {inc_path}",
        echo=verbose,
    )

    c.run(f"python scripts/generate_init.py --dir {inc_path}", echo=verbose)

    version_toml = Path("version.toml")
    c.run(
        f"python scripts/generate_version.py --io {version_toml} --dir {inc_path}",
        echo=verbose,
    )


def _meson_source(c, vars, verbose):
    """
    Automatically detects and creates a list of source and include files for the build. The list of
    files can be found in the build path.

    Undertstood that this is often seen as bad practice. However, this system has a rigid directory
    and naming structure, and doing this facilitates adding new files to the system.
    """

    c.run(
        f"python scripts/generate_source.py --build_dir {vars["build_dir"]} --driver {vars['driver']} --rtos {vars['os'].lower()} --platform {vars['platform']}",
        echo=verbose,
    )


def _meson_setup(c, vars, verbose, cross_compile, options):
    """
    Configures, or re-configures, meson build.
    """

    platform_path = Path("platforms") / Path(vars["platform"])
    sys_file = str(platform_path / Path("system.ini"))
    sys_spec = f"{'--cross-file' if cross_compile else '--native-file'} {sys_file}"
    build_path = str(Path(vars["build_dir"]) / Path(vars["build_name"]))

    if not Path(build_path).is_dir():
        c.run(
            f"meson setup {sys_spec} {build_path} {' '.join(options)}",
            echo=verbose,
            pty=True,
        )
    else:
        c.run(
            f"meson setup --reconfigure {sys_spec} {build_path} {' '.join(options)}",
            echo=verbose,
            pty=True,
        )


def _meson_compile(c, vars, verbose):
    """
    Compiles the build, and copies the resulting compile commnads to the top-level build directory.
    """

    build_path = str(Path(vars["build_dir"]) / Path(vars["build_name"]))
    c.run(
        f"meson compile {'--verbose --ninja-args=-d,explain' if verbose else ''} -C {build_path}",
        echo=verbose,
        pty=True,
    )

    c.run(f"cp {Path(build_path) / Path("compile_commands.json")} build", echo=verbose)


def _build(c, vars, verbose, cross_compile=True, additional_options=[]):
    """
    Builds the application.
    """

    _subproject_deps(c, verbose)

    options = [
        f"-Dplatform={vars['platform'].upper()}",
        f"-Dversion={vars['version']}",
        f"-Drtos={vars['os']}",
        f"-Ddriver={vars['driver']}",
        "-Dsave-temps=true" if vars["temps"] else "",
    ]

    options += additional_options

    try:
        c.run(f"mkdir {vars['build_dir']}")
    except UnexpectedExit:
        pass

    with open(Path(vars["build_dir"]) / Path("last_build.txt"), "w") as file:
        file.write(vars["build_name"])

    _meson_source(c, vars, verbose)

    _meson_setup(c, vars, verbose, cross_compile, options)

    _generate_code(c, vars, verbose)

    _meson_compile(c, vars, verbose)


def _test(c, vars, verbose=False, coverage=False):
    """
    Builds and runs the unit tests.
    """

    _build(
        c,
        vars,
        verbose,
        False,
        [
            f"-Db_coverage=true",
            f"-Dbuildtype=debug",
            f"-Dtest=true",
        ],
    )

    build_path = str(Path(vars["build_dir"]) / Path(vars["build_name"]))

    c.run(
        f"meson test -C {build_path} --print-errorlogs --no-rebuild {'--verbose' if verbose else ''}",
        echo=verbose,
        pty=True,
    )

    if not coverage:
        return

    c.run(f"ninja coverage -C {build_path}", echo=verbose)


@task(
    help={
        "verbose": "Verbose output.",
    }
)
def clean(c, verbose=False):
    """
    Removes all generated directories and files.
    """

    vars = _get_vars(c)

    c.run(f"rm -rf {vars['build_dir']}", echo=verbose)
    c.run(f"rm -rf subprojects", echo=verbose)


@task(
    help={
        "verbose": "Verbose output.",
        "device": "Specifies the device to debug. Currently supported devices: STM32G474RE, S32K311.",
    }
)
def debug(c, verbose=False, tool="jlink", device="versatilepb"):
    """
    Launches a debug session on the hardware.
    """

    vars = _get_vars(c)

    if tool == "jlink":
        promise = c.run(
            f"{vars['jlink_gdb']} -device {device} -speed 4000 -if SWD -port 61234",
            asynchronous=True,
            echo=verbose,
        )
        sleep(1)  # Give the server time to start
        c.run(
            (
                f'arm-none-eabi-gdb {vars["build_dir"]}/{vars["last_built"]}/HyperLynk_Thermal_Control.elf '
                f'-ex "target remote localhost:61234"'
            ),
            pty=True,
            echo=verbose,
        )
        promise.runner.kill()
    else:
        print(f"Debug tool {tool} not recognized.")


@task(
    help={
        "verbose": "Verbose output.",
        "tool": "Specifies the tool used to flash the debugger.",
        "device": "Specifies the device to flash. Currently supported devices: STM32G474RE, S32K311.",
    }
)
def flash(c, verbose=False, tool="qemu", device="versatilepb"):
    """
    Flashes the device with the last application built.
    """

    vars = _get_vars(c)
    if vars["last_built"] == None:
        print("No build found")
        return

    build_path = str(Path(vars["build_dir"]) / Path(vars["last_built"]))

    if device == "versatilepb":
        address = "0x10000"

    if tool == "jlink":
        c.run(
            f'echo -e "si 1\ndevice {device}\nspeed 4000\nr\nloadbin {build_path}/app.bin, {address}\nr\ng\nexit\n" | {vars["jlink"]}',
            echo=verbose,
        )
    elif tool == "qemu":
        c.run(
            f"qemu-system-arm -M versatilepb -m 128M -nographic -kernel {build_path}/app.bin"
        )
    else:
        print(f"Flash tool {tool} unrecognized.")


@task(
    help={
        "verbose": "Verbose output.",
    }
)
def doxy(c, verbose=False):
    """
    Generates doxygen documentation from the code.
    """

    vars = _get_vars(c)

    c.run(f"{vars['doxy']} doxyfile", echo=verbose)


@task(
    help={
        "verbose": "Verbose output.",
    }
)
def format(c, verbose=False):
    """
    Runs the formatter on all C/C++ files.
    """

    str = c.run(
        'find \\( -path "./build" \\) -prune -false -o -iname *.cpp -o -iname *.c -o -iname *.h -o -iname *.hpp',
        echo=verbose,
        hide="out",
    ).stdout

    for line in str.split("\n"):
        if line == "":
            continue

        c.run(
            f"clang-format -i -style=file -fallback-style=LLVM --Wno-error=unknown {line}",
            echo=verbose,
        )


@task(
    help={
        "verbose": "Verbose output.",
        "port": "The port to use for the system tests.",
        "color": "Colorizes the output. Does not work on all systems.",
    }
)
def systest(c, verbose=False, port="/dev/ttyUSB0", color=False):
    """
    Runs the system tests. See scripts/README.md for more details.

    You will need to make an invoke.yaml file that contains your sudo password, or else you will be
    prompted for it, in order to give permissions to the port. See
    https://docs.pyinvoke.org/en/stable/concepts/configuration.html for more info.
    """

    try:
        c.sudo(f"chmod a+rwx {port}", echo=verbose)
    except Exception:
        pass

    c.run(
        f"PYTHONDONTWRITEBYTECODE=1 pytest -v -p no:cacheprovider test/system/ --port {port}",
        echo=verbose,
        pty=color,
    )


@task(
    help={
        "verbose": "Verbose output.",
    }
)
def comm(c, verbose=False, port="/dev/ttyUSB0"):
    """
    Attempts to start a comm session with the UART CLI of the device.

    You will need to make an invoke.yaml file that contains your sudo password, or else you will be
    prompted for it, in order to give permissions to the port. See
    https://docs.pyinvoke.org/en/stable/concepts/configuration.html for more info.
    """

    try:
        c.sudo(f"chmod a+rwx {port}", echo=verbose)
    except Exception:
        pass

    run(f"minicom --baudrate 115200 --device {port}", shell=True)


@task(
    help={
        "verbose": "Verbose output.",
    }
)
def regen(c, verbose=False):
    """
    Regenerates the auto-generated functions in the tasks.py file.
    """

    c.run("python scripts/generate_builds.py", echo=verbose)


# -------------------------- AUTO-GENERATED BELOW THIS LINE ---------------------------------------
# ---------------------------------- DO NOT MODIFY ------------------------------------------------


@task(
    help={
        "verbose": "Print verbose output",
        "temps": "Preprocessor and disassembly output",
    }
)
def versatilepb(c, verbose=False, temps=False):
    """
    Build for versatilepb
    """
    vars = _get_vars(c)

    vars["build_name"] = "versatilepb"
    vars["temps"] = temps
    vars["os"] = "FreeRTOS"
    vars["driver"] = "versatilepb"
    vars["platform"] = "versatilepb_qemu"
    vars["version"] = "0.0.0"

    _build(c, vars, verbose)


@task(
    help={
        "verbose": "Print verbose output",
        "coverage": "Generates code coverage report",
    }
)
def unitest(c, verbose=False, coverage=False):
    """
    Build for unitest
    """
    vars = _get_vars(c)

    vars["build_name"] = "unitest"
    vars["temps"] = False
    vars["os"] = "FreeRTOS"
    vars["driver"] = "versatilepb"
    vars["platform"] = "unit_test"
    vars["version"] = "0.0.0"

    _test(c, vars, verbose, coverage)


#EOF