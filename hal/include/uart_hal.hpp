/// @file uart_hal.hpp
/// @author Denver Hoggatt
/// @brief UART HAL declarations
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#pragma once

#include "error.hpp"
#include "hal.hpp"
#include "uart.hpp"

#include <cstdint>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------


namespace uart_hal
{
    //----------------------------------------------------------------------------------------------
    //  Public Constants
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Classes
    //----------------------------------------------------------------------------------------------

    class UARTHAL : public hal::HAL
    {
        private:
            // -----------------------------------------------------------------
            //  Class Private Variables
            // -----------------------------------------------------------------


            // -----------------------------------------------------------------
            //  Class Private Functions
            // -----------------------------------------------------------------


        public:
            // -----------------------------------------------------------------
            //  Class Public Variables
            // -----------------------------------------------------------------


            // -----------------------------------------------------------------
            //  Class Public Functions
            // -----------------------------------------------------------------

            error::Error send(void *handle, const char *send_str);

            error::Error open(void *handle);

            // End of Class
    };

    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    error::Error send(uart::VirtualPort id, const char *send_str);

    error::Error open(uart::VirtualPort id);

    void init();

#define DEF_PLAT(plat_name) UARTHAL *plat_name##_get_funcs();
#include "platforms.def"
#undef DEF_PLAT

    // End of Namespace
}

// End of File