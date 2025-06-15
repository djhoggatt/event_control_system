# Tests

This system follows a specific naming structure for unit tets to allow for easy test creation. Please follow the instructions below.


## Adding a Unit Test

To add a unit test:
* Create a new cpp file for the tests in the unit/source directory.
* The name of this file must conform to the following standard: MODULE_test.cpp.
* Where MODULE is the name of the cpp file to test.


## Unit Test Accessors

Sometimes a unit test need access to information within the module's translation unit. To access this informatioin:
* Create appropriate accessor functions in the MODULE_test namespace within the module's cpp.
* Create a header file MODULE_test.hpp in the unit/include directory.
* Declare the accessor function in that header file.
* Include the header file in the test file.


## Running a Unit Test

To run the unit tests:
* Run `invoke unitest` from the command line. This will both build, and run all of the tests.
* Run `invoke unitest --coverage` from the command line to see code coverage.


## Adding a System Test

A system test is a test that runs on actual hardware. This requires having a hardware setup, and connected to the machine running the tests.

To add a system test:
* Add a python script in the system directory.
* This script must be named NAME_test.py.
* Where NAME is simply the name of the test.


## Running a System Test

A system test is a test that runs on actual hardware. This requires having a hardware setup, and connected to the machine running the tests.

To run the system tests:
* Run `invoke systest` from the command line. This will run all of the system tests.