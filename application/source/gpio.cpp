/// @file gpio.cpp
/// @author Denver Hoggatt
/// @brief GPIO definitions
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "gpio.hpp"
#include "gpio_hal.hpp"
#include "io.hpp"
#include "macros.hpp"

#include <cstring>
#include <cstdio>
#include <cinttypes>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------

namespace
{
    //----------------------------------------------------------------------------------------------
    //  Private Constants
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Private Data Types
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Private Function Prototypes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  File Variables
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------


    // End of Anonymous Namespace
}

namespace gpio
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Class Function Definitions
    //----------------------------------------------------------------------------------------------

    void GPIO::print(void *data, io::IODirection dir)
    {
        char data_str[1 + 1]; // +1 \0
        sprintf(data_str, "%01d", (bool)data);

        io::print("GPIO", this->name, this->id, data_str, dir);
    }

    void *GPIO::get_by_id()
    {
        bool ret_val = gpio_hal::read(this->gpio_port);

        return (void *)ret_val;
    }

    void GPIO::set_output(void *data)
    {
        bool enable = (bool)data;

        error::Error err = error::NoError;

        if (enable)
        {
            err = gpio_hal::set(this->gpio_port);
        }
        else
        {
            err = gpio_hal::reset(this->gpio_port);
        }

        ENSURE(err == error::NoError, error::DeviceFailed);
    }

    void GPIO::init()
    {
        REENTRY_GUARD_CLASS();

        this->init_input_info(&typeid(bool), io::IOType::GPIO);
        this->init_output_info(&typeid(bool), io::IOType::GPIO);
    }

    //----------------------------------------------------------------------------------------------
    //  Class Operator Definitions
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Class Constructor Definitions
    //----------------------------------------------------------------------------------------------


    // End of Namespace
}

//--------------------------------------------------------------------------------------------------
// Global Namespace Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Unit Test Accessors
//--------------------------------------------------------------------------------------------------

namespace gpio_test
{


}

// End of File