"""
Author: Denver Hoggatt
Description: Generates the builds in python's tasks.py

Copyright (c) 2025 Denver Hoggatt. All rights reserved.

This software is licensed under the terms stated in the LICENSE file
located at the root of this repository. If no LICENSE file accompanies
this software, it is provided "AS IS" WITHOUT WARRANTY OF ANY KIND.
"""

# --------------------------------------------------------------------------------------------------
#  Imports
# --------------------------------------------------------------------------------------------------

import argparse
import tomllib
from pathlib import Path

# --------------------------------------------------------------------------------------------------
# Global Constants
# --------------------------------------------------------------------------------------------------

BUILD_FILE = Path("builds.toml")
INVOKE_FILE = Path("tasks.py")

DO_NOT_MODIFY_LINE = "# ---------------------------------- DO NOT MODIFY ------------------------------------------------"

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


def _make_build_func() -> str:
    with open(Path.cwd() / BUILD_FILE) as file:
        builds = tomllib.loads(file.read())

    build_func_str = ""

    for build_name in builds.keys():
        if "test" in build_name:
            continue

        build_func_str += f"\n@task(\n{TAB}help={{\n"
        build_func_str += f'{TAB}{TAB}"verbose": "Print verbose output",\n'
        build_func_str += f'{TAB}{TAB}"temps": "Preprocessor and disassembly output",\n'
        build_func_str += f"{TAB}}}\n)\n"
        build_func_str += f"def {build_name}(c, verbose=False, temps=False):\n"
        build_func_str += f'{TAB}"""\n{TAB}Build for {build_name}\n{TAB}"""\n'
        build_func_str += f"{TAB}vars = _get_vars(c)\n"
        build_func_str += "\n"
        build_func_str += f'{TAB}vars["build_name"] = "{build_name}"\n'
        build_func_str += f'{TAB}vars["temps"] = temps\n'
        for item in builds[build_name].keys():
            build_func_str += f'{TAB}vars["{item}"] = "{builds[build_name][item]}"\n'
        build_func_str += "\n"
        build_func_str += f"{TAB}_build(c, vars, verbose)\n"
        build_func_str += "\n"

    return build_func_str


def _make_test_func() -> str:
    with open(Path.cwd() / BUILD_FILE) as file:
        builds = tomllib.loads(file.read())

    build_func_str = ""

    for build_name in builds.keys():
        if "test" not in build_name:
            continue

        build_func_str += f"\n@task(\n{TAB}help={{\n"
        build_func_str += f'{TAB}{TAB}"verbose": "Print verbose output",\n'
        build_func_str += f'{TAB}{TAB}"coverage": "Generates code coverage report",\n'
        build_func_str += f"{TAB}}}\n)\n"
        build_func_str += f"def {build_name}(c, verbose=False, coverage=False):\n"
        build_func_str += f'{TAB}"""\n{TAB}Build for {build_name}\n{TAB}"""\n'
        build_func_str += f"{TAB}vars = _get_vars(c)\n"
        build_func_str += "\n"
        build_func_str += f'{TAB}vars["build_name"] = "{build_name}"\n'
        build_func_str += f'{TAB}vars["temps"] = False\n'
        for item in builds[build_name].keys():
            build_func_str += f'{TAB}vars["{item}"] = "{builds[build_name][item]}"\n'
        build_func_str += "\n"
        build_func_str += f"{TAB}_test(c, vars, verbose, coverage)\n"
        build_func_str += "\n"

    return build_func_str


def main():
    """
    Parses the TOML file and updates the tasks.py file.
    """

    file_to_write = ""

    with open(Path.cwd() / INVOKE_FILE, "r") as file:
        for line in file.readlines():
            file_to_write += line
            if DO_NOT_MODIFY_LINE in line:
                break

    file_to_write += "\n"
    file_to_write += _make_build_func()
    file_to_write += _make_test_func()
    file_to_write += "\n#EOF"

    with open(Path.cwd() / INVOKE_FILE, "w") as file:
        file.write(file_to_write)


# --------------------------------------------------------------------------------------------------
# Script
# --------------------------------------------------------------------------------------------------

if __name__ == "__main__":
    main()

# End of File
