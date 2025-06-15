/// @file uart_hal.cpp
/// @author Denver Hoggatt
/// @brief UART HAL definitions
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "uart_hal.hpp"
#include "macros.hpp"

#include <cstring>
#include <cstdio>

#define INCLUDE_PLATFORM_HEADERS
#include "platforms.def"

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

    uart_hal::UARTHAL *uart_hals[(uint32_t)hal::Platform::NumPlatforms];

    void
        *uart_handles[(uint32_t)hal::Platform::NumPlatforms][(uint32_t)uart::VirtualPort::NumPorts];

    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------


    // End of Anonymous Namespace
}

namespace uart_hal
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    error::Error send(uart::VirtualPort id, const char *send_str)
    {
        uint32_t plat = hal::platform();
        if (uart_hals[plat] == nullptr)
        {
            return error::NoError;
        }

        error::Error ret_val = error::InvalidPointer;

        if (id >= uart::VirtualPort::NumPorts)
        {
            ret_val = error::InvalidID;
        }
        else
        {
            ret_val = uart_hals[plat]->send(uart_handles[plat][(uint32_t)id], send_str);
        }

        return ret_val;
    }

    error::Error open(uart::VirtualPort id)
    {
        uint32_t plat = hal::platform();
        if (uart_hals[plat] == nullptr)
        {
            return error::NoError;
        }

        error::Error ret_val = error::InvalidPointer;

        if (id >= uart::VirtualPort::NumPorts)
        {
            ret_val = error::InvalidID;
        }
        else
        {
            ret_val = uart_hals[plat]->open(uart_handles[plat][(uint32_t)id]);
        }

        return ret_val;
    }

    void init()
    {
        memset(uart_handles, 0, sizeof(uart_handles));

#define DEF_PLAT(plat_name) uart_hals[(uint32_t)hal::Platform::plat_name] = plat_name##_get_funcs();
#include "platforms.def"
#undef DEF_PLAT

#define DEF_UART(PLAT, VIRT_PIN, PHY_PIN, PHY_PORT, ACTIVE, HANDLE)                    \
    uart_handles[(uint32_t)hal::Platform::PLAT][(uint32_t)uart::VirtualPort::VIRT_PIN] \
        = (void *)(HANDLE);
#include "platforms.def"
#undef DEF_UART
    }

    //----------------------------------------------------------------------------------------------
    //  Class Function Definitions
    //----------------------------------------------------------------------------------------------


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

namespace uart_Hal_test
{


}

// End of File