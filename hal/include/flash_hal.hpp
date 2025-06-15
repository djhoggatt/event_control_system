/// @file flash_hal.hpp
/// @author Denver Hoggatt
/// @brief Flash HAL declarations
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

#include <cstdint>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------


namespace flash_hal
{
    //----------------------------------------------------------------------------------------------
    //  Public Constants
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Data Types
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Classes
    //----------------------------------------------------------------------------------------------

    class FlashHAL : public hal::HAL
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

            error::Error read(uintptr_t addr, uint8_t *buf, uint32_t size);

            error::Error write(uintptr_t addr, uint8_t *buf, uint32_t size);

            error::Error erase(uintptr_t addr);

            uint32_t align(uint32_t val);

            uint32_t sector_size();

            // End of Class
    };

    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    error::Error read(uint32_t offset, uint8_t *buf, uint32_t size);

    error::Error write(uint32_t offset, uint8_t *buf, uint32_t size);

    error::Error erase(uint32_t offset);

    uint32_t align(uint32_t val);

    uint32_t get_sector_size();

    void init();

#define DEF_PLAT(plat_name) FlashHAL *plat_name##_get_funcs();
#include "platforms.def"
#undef DEF_PLAT

    // End of Namespace
}

// End of File