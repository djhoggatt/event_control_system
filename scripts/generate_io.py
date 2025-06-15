"""
Author: Denver Hoggatt
Description: Generates statically allocated IO classes, and creates a function for initializing them.

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

LIST_FILE_NAME = "io_list.cpp"
ID_FILE_NAME = "io_id.hpp"

HEADER_LIST_HEAD = '\
// THIS IS AN AUTO-GENERATED FILE, DO NOT MODIFY\n\
// clang-format off\n\
\n\
#include <cinttypes>\n\
\n\
#include "io.hpp"\n\
#include "input.hpp"\n\
#include "output.hpp"\n\
#include "{}"\n\
\n\
'.format(
    ID_FILE_NAME
)
HEADER_LIST_FOOT = "\
\n\
// clang-format on\
"

HEADER_ID_HEAD = "\
// THIS IS AN AUTO-GENERATED FILE, DO NOT MODIFY\n\
// clang-format off\n\n\
#pragma once\n\n\
namespace io\n\
{\n\
"
HEADER_ID_FOOT = "\
// clang-format on\
"

TAB = "    "

INPUTS = ["ADC"]

OUTPUTS = []

INPUTS_OUTPUTS = ["UART", "GPIO"]

# --------------------------------------------------------------------------------------------------
# Global Variables
# --------------------------------------------------------------------------------------------------

ids: str = (
    f"{TAB}enum class IOID\n{TAB}{{\n{TAB}{TAB}InvalidID = 0, // Must be zero for error checking\n"
)
instance_list: str = "\n"
name_list: str = ""
def_list: str = f"static void init_io()\n{{"
input_list: str = f"static input::Input *io_input_list[] =\n{{\n"
output_list: str = f"static output::Output *io_output_list[] =\n{{\n"
type_list: str = f"static io::IOType io_type_list[] =\n{{\n"
name_list_size: int = 0
type: str = ""
includes_list: str = ""

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


def _assign_param(io_name, param_name, param_value):
    """
    Generates code for assigning the given parameter to the given IO.
    """

    # Don't add children nodes, just values
    if isinstance(param_value, dict):
        return ""

    ret_val = ""

    if param_name == "type":
        ret_val = f"{TAB}{io_name}.{param_name} = io::IOType::{param_value};\n"
    elif (len(param_name.split("_")) > 1) and (param_name.split("_")[1] == "port"):
        port_type = param_name.split("_")[0]
        ret_val = (
            f"{TAB}{io_name}.{param_name} = {port_type}::VirtualPort::{param_value};\n"
        )
    else:
        ret_val = f"{TAB}{io_name}.{param_name} = {param_value};\n"

    return ret_val


def assign_params(io_name, io_obj, parent):
    """
    Generates code which assigns all of the params specified in the TOML file, for an IO.
    """

    ret_val = [_assign_param(io_name, param, io_obj[param]) for param in io_obj.keys()]

    ret_val += f"{TAB}{io_name}.parent = {parent};\n"

    return "".join(ret_val)


def add_top_level_device(io_name, io_obj):
    """
    Adds a parent device to the generated code.
    """

    global ids
    global includes_list
    global type_list
    global instance_list
    global name_list
    global name_list_size
    global def_list
    global input_list
    global output_list

    ids += f"{TAB}{TAB}{io_name.upper()},\n"

    type = io_obj["type"]
    type_list += f"{TAB}io::IOType::{type},\n"

    includes_list += f'#include "{type.lower()}.hpp"\n'

    instance_list += f"static {type.lower()}::{type} {io_name.lower()};\n"

    name_list += f'static const char *{io_name.lower()}_name = "{io_name.lower()}";\n'
    name_list_size += len(io_name.lower())

    def_list += "\n"
    def_list += f"{TAB}{io_name.lower()}.id = io::IOID::{io_name.upper()};\n"
    def_list += f"{TAB}{io_name.lower()}.name = {io_name.lower()}_name;\n"
    def_list += assign_params(io_name.lower(), io_obj, f"nullptr")

    if type in INPUTS:  # Based on values specified in io.hpp
        input_list += f"{TAB}&{io_name.lower()},\n"
    elif type in OUTPUTS:
        output_list += f"{TAB}&{io_name.lower()},\n"
    elif type in INPUTS_OUTPUTS:
        input_list += f"{TAB}&{io_name.lower()},\n"
        output_list += f"{TAB}&{io_name.lower()},\n"
    else:
        print(f"Error: Unrecognized direction for {io_name}")

    return


def add_lower_level_device(parent_name, io_name, io_obj):
    """
    Adds a child device to the generated code.
    """

    global def_list

    def_list += assign_params(io_name, io_obj, f"&{parent_name}")


def add_device_recursive(io_list: dict[str, any], parent_name=None):
    """
    Recursively adds the given device, and all child devices to the generated code.

    Recursion allows for any number of child devices to be in the IO device tree.
    """

    for key in io_list.keys():
        is_device = isinstance(io_list[key], dict)
        if not is_device:
            continue

        if parent_name == None:
            io_name = key.lower()
            add_top_level_device(io_name, io_list[key])
        else:
            io_name = parent_name + "." + key.lower()
            add_lower_level_device(parent_name, io_name, io_list[key])

        add_device_recursive(io_list[key], io_name)


def main(io_file_name, output_dir_name):
    """
    Converts the IO TOML file into auto-generated code.
    """

    global ids
    global includes_list
    global instance_list
    global name_list
    global def_list
    global input_list
    global output_list
    global type_list
    global name_list_size

    Path(output_dir_name).mkdir(parents=True, exist_ok=True)

    with open(io_file_name) as file:
        io_list = tomllib.loads(file.read())

    add_device_recursive(io_list)

    ids += f"{TAB}{TAB}NumIDs,\n{TAB}}};\n}}\n\n"
    instance_list += "\n"
    def_list += f"}}\n\n"
    input_list += f"}};\n\n"
    output_list += f"}};\n\n"
    type_list += f"}};\n\n"
    name_list += f"\n"
    name_list_size_def = (
        f"constexpr uint32_t TOTAL_IO_NAME_SIZE = {name_list_size};\n\n"
    )

    id_file = HEADER_ID_HEAD + ids + name_list_size_def + HEADER_ID_FOOT

    _write_file(Path(output_dir_name) / Path(ID_FILE_NAME), id_file)

    list_file = (
        HEADER_LIST_HEAD
        + includes_list
        + "\n"
        + instance_list
        + name_list
        + input_list
        + output_list
        + type_list
        + def_list
        + HEADER_LIST_FOOT
    )
    _write_file(Path(output_dir_name) / Path(LIST_FILE_NAME), list_file)


# --------------------------------------------------------------------------------------------------
# Script
# --------------------------------------------------------------------------------------------------

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--io", type=str, required=True)
    parser.add_argument("--dir", type=str, required=True)
    args = parser.parse_args()

    main(args.io, args.dir)

# End of File
