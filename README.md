# Generic Event-Driven Control System

This system is intended to act as a generic event-driven control system that can be expanded and templated off of.

There are three lists:
1. An input list
2. A control list
3. An output list

The input list takes in input values from hardware, and signals that an input has been received via events.

The control list takes in events, either from inputs or other controls, and then sets outputs.

The output list takes in output values from controls, and sets the appropriate values in hardware.


## Directory Structure

This sytem assumes a strict directory and naming structure to make adding new files to the build easier. Please follow this structure:

* **application** - Contains files associated with the application.
    * *include* - Contains application header files.
    * *source* - Contains application code files.
* **drivers** - Contains files associated with the MCU.
    * *~MCU~* - MCU that this device can run on.
        * *include* - Contains MCU-level header files.
        * *source* - Contains MCU-level code files.
        * *startup* - Contains startup assembly code and linker files.
* **hal** - Contains files associated with the hardware (i.e. board-level, or calls into the drivers).
    * *include* - Contains HAL header files.
    * *source* - Contains HAL code files.
* **osal** - Contains files associated with the operating system.
    * *Inc* - Contains OSAL header files.
    * *Src* - Contains OSAL code files.
* **platform** - Contains files associated with the platform setup.
    * *~PLATFORM~* - Platform (hardware) that this device runs on.
* **rtos** - Contains files associated with the operating system.
    * *~RTOS~* - Operating system that this device utilizes.
* **script** - Contains various scripts used by the system.
* **templates** - Contains template files.
* **test** - Contains files associated with testing.
    * *system* - Contains files associated with system (HIL) testing.
    * *unit* - Contains files associated with unit testing.
        * *include* - Contains unit test header files.
        * *source* - Contains unit test code files.


## Abstraction Layers

In embedded systems, it is often important to abstract out a hardware layer from an application layer, which makes it easier to port the system to a new MCU or board.

This systems uses a class-based HAL implementation, where the class and abstract-level functions are implemented in a file titled NAME_hal.cpp/hpp, and the hardware-level functions are implemented in NAME_hal_PLATFORM.cpp.

A similar class-based system is used to abstract out the operating system. Class and abstract-level functions are implemented in a file titles NAME_osal.cpp/hpp, and the operating system specific functions are implemented in NAME_osal_RTOS.cpp.


## Device Tree

This system has a unique device tree implementation for all IO objects. This tree can be found in the associated platform `io.toml` file.

You can specify parent objects with the entry [OBJ]. This will generate a statically allocated instance and identifier for that object.

You can specify child objects (e.g. IO classes contained within another IO class) by using a period in the table entry, e.g. [OBJ.OBJ]. This can recurse as much as needed to specify all IO, e.g. [OBJ.OBJ.OBJ].

Generated identifiers are always upper case. Generated instances are always lower case. So if you have A child object, you will need to specify it in upper-case, even if the object name is lowercase. E.g. `class Obj { IO adc; }`, will be specified as [OBJ.ADC], even though adc is lower-case in the class.

Top-level parent objects must always have a type specified. Any assignment that ends in _port will assign to a VirtualPort enumeration (which should be defined in the associated header file). All other assignments can be anything that you want, they will be interpreted directly into code.


## CLI

A very basic CLI has been implemented as an example control that can be used in this system.

This CLI has basic tab-completion, and support for up and down arrows, though it currently only keeps a history of the last command.

To use the CLI, boot up the system, and type "help" to see a list of commands.


## Invoke

This system uses python's invoke for build organization.

* To build the software, run: `invoke {build}`, where *build* is the code you want to build. For example:
    - To build the default controls system that runs on QEMU's versatilepb, run: `invoke versatilepb`
* To run the unit tests, run: `invoke unitest`
* To run the system tests, run: `invoke systest`
* To delete all build files, run: `invoke clean`
* Add --help to any of the above command for more information, including additional optional settings.
* To see a full list of possible commands, from the command line, run: `invoke --list`

For tab completion, run the command `invoke --print-completion-script SHELL`, where SHELL is the CLI you are using (e.g. bash), and copy the resulting output into your startup script.


## Builds

Builds are specified in the `builds.toml` file. New builds can be created by adding a new entry into this file.


## Settings

The system also has support for storing persistent values. The list of settings can be found in settings.def.

Any module that is specified in settings.def must support a set_param and get_param function. The function set_param will be called when a settings is either set by the user, or loaded from persistent memory on bootup. The function get_param will be called when a setting is accessed.


<!-- AUTOGENERATED - DO NOT MAKE MODIFICATIONS BELOW THIS LINE -->


## Links
**[Generic Event-Driven Control System](README.md)**<br>
**[Tests](test/README.md)**<br>
