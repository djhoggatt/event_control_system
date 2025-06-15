/// @file template.hpp
/// @author Denver Hoggatt
/// @brief Brief description of the file
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#pragma once

#include "input.hpp"
#include "output.hpp"
#include "error.hpp"

#include <cstdint>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------


namespace gpio
{
    //----------------------------------------------------------------------------------------------
    //  Public Constants
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Data Types
    //----------------------------------------------------------------------------------------------

    enum class VirtualPort : uint32_t
    {
        GPIO_1,

        NumPorts,
    };

    class GPIO
        : public input::Input
        , public output::Output
    {
        private:
            // -----------------------------------------------------------------
            //  Class Private Variables
            // -----------------------------------------------------------------


            // -----------------------------------------------------------------
            //  Class Private Functions
            // -----------------------------------------------------------------

            void set_output(void *data);

            void *get_by_id();

            void print(void *data, io::IODirection dir);

        public:
            // -----------------------------------------------------------------
            //  Class Public Variables
            // -----------------------------------------------------------------

            VirtualPort gpio_port;

            // -----------------------------------------------------------------
            //  Class Public Functions
            // -----------------------------------------------------------------

            void init();

            // End of Class
    };

    //----------------------------------------------------------------------------------------------
    //  Classes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------


    // End of Namespace
}

// End of File