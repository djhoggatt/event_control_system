"""
Author: Denver Hoggatt
Description: Generates a meson.build file with the sources and includes for the build.

Copyright (c) 2025 Denver Hoggatt. All rights reserved.

This software is licensed under the terms stated in the LICENSE file
located at the root of this repository. If no LICENSE file accompanies
this software, it is provided "AS IS" WITHOUT WARRANTY OF ANY KIND.
"""

# --------------------------------------------------------------------------------------------------
#  Imports
# --------------------------------------------------------------------------------------------------

from argparse import ArgumentParser
from pathlib import Path

# --------------------------------------------------------------------------------------------------
# Global Constants
# --------------------------------------------------------------------------------------------------

DIRECTORIES = [
    "application",
    "hal",
    "osal",
    "rtos/freertos",
]

DRIVERS = [
    "versatilepb",
]

SRC_DIR = "source"
INC_DIR = "include"

DRIVER_DIR = "drivers"
START_DIR = "startup"

HAL_DIR = "hal"

OSAL_DIR = "osal"

PLAT_DIR = "platforms"

RTOS_DIR = "rtos"
PORT_DIR = "port"

TEST_DIR = "test/unit"

HEADER = "# THIS IS AN AUTO-GENERATED FILE, DO NOT MODIFY\n\n"

SRC_LIST_HEADER = "project_src = [\n"
INC_LIST_HEADER = "project_inc = [\n"
TEST_LIST_HEADER = "test_src = [\n"
TEST_INC_LIST_HEADER = "test_inc = [\n"

LIST_FOOTER = "]\n\n"

TAB = "    "

# --------------------------------------------------------------------------------------------------
# Global Variables
# --------------------------------------------------------------------------------------------------


# --------------------------------------------------------------------------------------------------
# Classes
# --------------------------------------------------------------------------------------------------


# --------------------------------------------------------------------------------------------------
# Functions
# --------------------------------------------------------------------------------------------------


def get_srcs_and_incs(driver: str, rtos: str, platform: str):
    """
    Get all the source files and include files needed to build.
    """

    sources: list[Path] = []
    includes: list[Path] = []

    for dir in DIRECTORIES:
        files = list((Path(dir) / Path("source")).glob("*.c*"))
        sources += [str(file) for file in files]
        includes.append(str(files[0].parent.parent / Path(INC_DIR)))

    driver_files = (Path(DRIVER_DIR) / Path(driver) / Path(SRC_DIR)).glob("*.c*")
    sources += [str(file) for file in driver_files]

    hal_files = (Path(HAL_DIR) / Path(platform)).glob("*.c*")
    sources += [str(file) for file in hal_files]

    main_file = Path(DRIVER_DIR) / Path(driver) / Path(START_DIR) / "main.c"
    sources.append(main_file)

    osal_files = (Path(OSAL_DIR) / Path(rtos)).glob("*.c*")
    sources += [str(file) for file in osal_files]

    rtos_port_path = Path(RTOS_DIR) / Path(rtos) / Path(PORT_DIR) / Path(driver)
    sources += [str(file) for file in rtos_port_path.glob("*.c*")]

    includes.append(Path(DRIVER_DIR) / Path(driver) / Path(INC_DIR))
    includes.append(rtos_port_path)
    includes.append(Path(PLAT_DIR) / Path(platform))

    return (sources, includes)


def get_test_srcs_and_incs():
    """
    Get all the source files and include files needed for the unit tests.
    """

    sources: list[Path] = []
    includes: list[Path] = []

    files = (Path(TEST_DIR) / Path(SRC_DIR)).glob("*_test.c*")
    sources += [str(file) for file in files]
    includes.append(str(Path(TEST_DIR) / Path(INC_DIR)))

    return (sources, includes)


def _write_file(file_name, content):
    """
    Writes the content to the file with the given name. If the file already exists
    and has the same content, it will not be overwritten.
    """
    write_file = False
    try:
        with open(file_name, "r") as file:
            write_file = content != file.read()
    except FileNotFoundError:
        write_file = True

    if write_file:
        with open(file_name, "w") as file:
            file.write(content)


def main(build_dir: str, driver: str, rtos: str, platform: str):
    """
    Main function.
    """

    output = HEADER

    (sources, includes) = get_srcs_and_incs(driver, rtos, platform)
    (test_sources, test_includes) = get_test_srcs_and_incs()

    source_list = SRC_LIST_HEADER
    for source in sources:
        source_list += f"{TAB}'{str(source)}',\n"

    include_list = INC_LIST_HEADER
    for includes in includes:
        include_list += f"{TAB}'{str(includes)}',\n"

    test_list = TEST_LIST_HEADER
    for source in test_sources:
        test_list += f"{TAB}'{str(source)}',\n"

    test_inc_list = TEST_INC_LIST_HEADER
    for include in test_includes:
        test_inc_list += f"{TAB}'{str(include)}',\n"

    output += source_list
    output += LIST_FOOTER
    output += include_list
    output += LIST_FOOTER
    output += test_list
    output += LIST_FOOTER
    output += test_inc_list
    output += LIST_FOOTER

    _write_file(Path(build_dir) / "meson.build", output)


# --------------------------------------------------------------------------------------------------
# Script
# --------------------------------------------------------------------------------------------------

if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument("--build_dir", type=str, required=True)
    parser.add_argument("--driver", type=str, required=False)
    parser.add_argument("--rtos", type=str, required=False)
    parser.add_argument("--platform", type=str, required=False)
    parser.set_defaults(include=False)
    args = parser.parse_args()

    try:
        driver = args.driver
    except AttributeError:
        driver = None

    try:
        rtos = args.rtos
    except AttributeError:
        rtos = None

    try:
        platform = args.platform
    except AttributeError:
        platform = None

    main(args.build_dir, driver, rtos, platform)

# End of File
