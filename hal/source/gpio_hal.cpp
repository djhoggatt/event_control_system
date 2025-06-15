/// @file gpio_hal.cpp
/// @author Denver Hoggatt
/// @brief GPIO HAL
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "hal.hpp"
#include "gpio_hal.hpp"
#include "io.hpp"
#include "macros.hpp"

#include <cstdint>
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

    uintptr_t gpio_pins[(uint32_t)hal::Platform::NumPlatforms]
                       [(uint32_t)gpio::VirtualPort::NumPorts];
    uintptr_t gpio_ports[(uint32_t)hal::Platform::NumPlatforms]
                        [(uint32_t)gpio::VirtualPort::NumPorts];
    gpio_hal::ActiveState gpio_actives[(uint32_t)hal::Platform::NumPlatforms]
                                      [(uint32_t)gpio::VirtualPort::NumPorts];
    gpio_hal::GPIOHAL *gpio_hals[(uint32_t)hal::Platform::NumPlatforms];

    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------


    // End of Anonymous Namespace
}

namespace gpio_hal
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    error::Error set(gpio::VirtualPort pin)
    {
        UNUSED(pin);

        uint32_t plat = hal::platform();

        error::Error ret_val = error::NoError;

        if (pin >= gpio::VirtualPort::NumPorts)
        {
            ret_val = error::InvalidPin;
        }
        else if (gpio_pins[plat][(uint32_t)pin] == UINT_MAX)
        {
            ret_val = error::NoError;
        }
        else if (gpio_hals[plat] != nullptr)
        {
            ret_val = gpio_hals[plat]->set(gpio_ports[plat][(uint32_t)pin],
                                           gpio_pins[plat][(uint32_t)pin],
                                           gpio_actives[plat][(uint32_t)pin]);
        }

        return ret_val;
    }

    error::Error reset(gpio::VirtualPort pin)
    {
        UNUSED(pin);

        uint32_t plat = hal::platform();

        error::Error ret_val = error::NoError;

        if (pin >= gpio::VirtualPort::NumPorts)
        {
            ret_val = error::InvalidPin;
        }
        else if (gpio_pins[plat][(uint32_t)pin] == UINT_MAX)
        {
            ret_val = error::NoError;
        }
        else if (gpio_hals[plat] != nullptr)
        {
            ret_val = gpio_hals[plat]->reset(gpio_ports[plat][(uint32_t)pin],
                                             gpio_pins[plat][(uint32_t)pin],
                                             gpio_actives[plat][(uint32_t)pin]);
        }

        return ret_val;
    }

    bool read(gpio::VirtualPort pin)
    {
        UNUSED(pin);

        uint32_t plat = hal::platform();

        bool ret_val = false;

        if (pin >= gpio::VirtualPort::NumPorts)
        {
            ret_val = false;
        }
        else if (gpio_pins[plat][(uint32_t)pin] == UINT_MAX)
        {
            ret_val = false;
        }
        else if (gpio_hals[plat] != nullptr)
        {
            ret_val = gpio_hals[plat]->read(gpio_ports[plat][(uint32_t)pin],
                                            gpio_pins[plat][(uint32_t)pin]);
        }

        return ret_val;
    }

    void init()
    {
#define DEF_PLAT(plat_name) gpio_hals[(uint32_t)hal::Platform::plat_name] = plat_name##_get_funcs();
#include "platforms.def"
#undef DEF_PLAT

        memset(gpio_pins, 0xFF, sizeof(gpio_pins));
        memset(gpio_ports, 0xFF, sizeof(gpio_ports));
        memset(gpio_actives, 0xFF, sizeof(gpio_actives));

#define DEF_GPIO(PLAT, VIRT_PIN, PHY_PIN, PHY_PORT, ACTIVE, HANDLE)                 \
    gpio_pins[(uint32_t)hal::Platform::PLAT][(uint32_t)gpio::VirtualPort::VIRT_PIN] \
        = (uintptr_t)PHY_PIN;
#include "platforms.def"
#undef DEF_GPIO

#define DEF_GPIO(PLAT, VIRT_PIN, PHY_PIN, PHY_PORT, ACTIVE, HANDLE)                  \
    gpio_ports[(uint32_t)hal::Platform::PLAT][(uint32_t)gpio::VirtualPort::VIRT_PIN] \
        = (uintptr_t)PHY_PORT;
#include "platforms.def"
#undef DEF_GPIO

#define DEF_GPIO(PLAT, VIRT_PIN, PHY_PIN, PHY_PORT, ACTIVE, HANDLE)                    \
    gpio_actives[(uint32_t)hal::Platform::PLAT][(uint32_t)gpio::VirtualPort::VIRT_PIN] \
        = (ActiveState)ACTIVE;
#include "platforms.def"
#undef DEF_GPIO
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

namespace gpio_hal_test
{


}

// End of File