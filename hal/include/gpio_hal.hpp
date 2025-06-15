
/// @file gpio_hal.hpp
/// @author Denver Hoggatt
/// @brief GPIO HAL declarations
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#pragma once

#include "hal.hpp"
#include "error.hpp"
#include "gpio.hpp"

#include <cstdint>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------


namespace gpio_hal
{
    //----------------------------------------------------------------------------------------------
    //  Public Constants
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Data Types
    //----------------------------------------------------------------------------------------------

    enum class ActiveState : int32_t
    {
        Unused = -1,
        Low    = 0,
        High   = 1
    };

    //----------------------------------------------------------------------------------------------
    //  Classes
    //----------------------------------------------------------------------------------------------

    class GPIOHAL : public hal::HAL
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

            bool read(uintptr_t port, uint32_t pin);

            error::Error reset(uintptr_t port, uint32_t pin, ActiveState active);

            error::Error set(uintptr_t port, uint32_t pin, ActiveState active);

            // End of Class
    };

    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    error::Error set(gpio::VirtualPort pin);

    error::Error reset(gpio::VirtualPort pin);

    bool read(gpio::VirtualPort pin);

    void init();

#define DEF_PLAT(plat_name) GPIOHAL *plat_name##_get_funcs();
#include "platforms.def"
#undef DEF_PLAT

    // End of Namespace
}

// End of File