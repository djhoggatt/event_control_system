"""
Author: Denver Hoggatt
Description: Generates initialization files.

Copyright (c) 2025 Denver Hoggatt. All rights reserved.

This software is licensed under the terms stated in the LICENSE file
located at the root of this repository. If no LICENSE file accompanies
this software, it is provided "AS IS" WITHOUT WARRANTY OF ANY KIND.
"""

# --------------------------------------------------------------------------------------------------
#  Imports
# --------------------------------------------------------------------------------------------------

from pathlib import Path
import re
import argparse

# --------------------------------------------------------------------------------------------------
# Global Constants
# --------------------------------------------------------------------------------------------------

HAL_DIR = "hal/source"
HAL_INIT_FILE_NAME = "hal_init.cpp"

HAL_INIT_HEAD = "// THIS IS AN AUTO-GENERATED FILE, DO NOT MODIFY\n"

HAL_INIT_FOOT = "\n"

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


def generate_hal(output_dir):
    src_dir = Path(HAL_DIR)
    pattern = re.compile(r"(.+)_hal\.cpp$")

    init_list = "static void hal_inits()\n{\n"
    inc_list = ""

    for file in src_dir.iterdir():
        match = pattern.fullmatch(file.name)
        if match:
            inc_list += f'#include "{match.group(1)}_hal.hpp"\n'
            init_list += f"{TAB}{match.group(1)}_hal::init();\n"

    inc_list += "\n"
    init_list += "}"

    file_contents = HAL_INIT_HEAD + inc_list + init_list + HAL_INIT_FOOT

    _write_file(Path(output_dir) / Path(HAL_INIT_FILE_NAME), file_contents)


# --------------------------------------------------------------------------------------------------
# Script
# --------------------------------------------------------------------------------------------------

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--dir", type=str, required=True)
    args = parser.parse_args()

    generate_hal(args.dir)

# End of File
