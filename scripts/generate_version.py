"""
Author: Denver Hoggatt
Description: Generates application version.

Copyright (c) 2025 Denver Hoggatt. All rights reserved.

This software is licensed under the terms stated in the LICENSE file
located at the root of this repository. If no LICENSE file accompanies
this software, it is provided "AS IS" WITHOUT WARRANTY OF ANY KIND.
"""

# --------------------------------------------------------------------------------------------------
#  Imports
# --------------------------------------------------------------------------------------------------

from argparse import ArgumentParser
from tomllib import load
from subprocess import check_output
from pathlib import Path
from platform import system

# --------------------------------------------------------------------------------------------------
# Global Constants
# --------------------------------------------------------------------------------------------------

VERSION_HEADER_FILE = "version_info.hpp"

HEADER_HEAD = "\
// THIS IS AN AUTO-GENERATED FILE, DO NOT MODIFY\n\
// clang-format off\n\
\n\
"
HEADER_FOOT = "\
\n\
// clang-format on\
"

# --------------------------------------------------------------------------------------------------
# Global Variables
# --------------------------------------------------------------------------------------------------


# --------------------------------------------------------------------------------------------------
# Classes
# --------------------------------------------------------------------------------------------------


# --------------------------------------------------------------------------------------------------
# Functions
# --------------------------------------------------------------------------------------------------


def get_commit_num():
    """
    Returns the short git hash for the current git revision
    """

    try:
        ret_val = check_output("git rev-parse --short=8 HEAD")
    except Exception:
        ret_val = "00000000"

    return ret_val


def main(version_toml_name: str, output_dir_name):
    """ """

    version_toml_path = str(Path.cwd() / version_toml_name)
    with open(version_toml_path, "rb") as version_toml:
        version_data = load(version_toml)

    major = version_data["major"]
    minor = version_data["minor"]
    patch = version_data["patch"]
    rev = get_commit_num()

    version_str = f"{major}.{minor}.{patch}-{rev}"

    str_declaration = f'static const char VERSION_STR[] = "{version_str}";\n'

    file_str = HEADER_HEAD + str_declaration + HEADER_FOOT

    with open(Path(output_dir_name) / Path(VERSION_HEADER_FILE), "w") as file:
        file.write(file_str)


# --------------------------------------------------------------------------------------------------
# Script
# --------------------------------------------------------------------------------------------------

if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument("--io", type=str, required=True)
    parser.add_argument("--dir", type=str, required=True)
    args = parser.parse_args()

    main(args.io, args.dir)

# End of File
