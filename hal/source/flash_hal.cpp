/// @file flash_hal.cpp
/// @author Denver Hoggatt
/// @brief Flash HAL
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "hal.hpp"
#include "flash_hal.hpp"

#include <cstdint>

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

    uint32_t             starting_phys_addr[(uint32_t)hal::Platform::NumPlatforms];
    flash_hal::FlashHAL *flash_hals[(uint32_t)hal::Platform::NumPlatforms];

    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------


    // End of Anonymous Namespace
}

namespace flash_hal
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    void init()
    {
#define DEF_PLAT(plat_name) \
    flash_hals[(uint32_t)hal::Platform::plat_name] = plat_name##_get_funcs();
#include "platforms.def"
#undef DEF_PLAT

#define DEF_FLASH(plat, addr) starting_phys_addr[(uint32_t)hal::Platform::plat] = addr;
#include "platforms.def"
#undef DEF_GPIO
    }

    error::Error read(uint32_t offset, uint8_t *buf, uint32_t size)
    {
        REQUIRE(buf, error::InvalidPointer);

        uint32_t plat = hal::platform();
        if (flash_hals[plat] == nullptr)
        {
            return error::NoError;
        }

        uint32_t addr = starting_phys_addr[(uint32_t)plat] + offset;

        return flash_hals[plat]->read(addr, buf, size);
    }

    error::Error write(uint32_t offset, uint8_t *buf, uint32_t size)
    {
        REQUIRE(buf, error::InvalidPointer);

        uint32_t plat = hal::platform();
        if (flash_hals[plat] == nullptr)
        {
            return error::NoError;
        }

        uint32_t addr = starting_phys_addr[(uint32_t)plat] + offset;

        return flash_hals[plat]->write(addr, buf, size);
    }

    error::Error erase(uint32_t offset)
    {
        uint32_t plat = hal::platform();
        if (flash_hals[plat] == nullptr)
        {
            return error::NoError;
        }

        uint32_t addr = starting_phys_addr[(uint32_t)plat] + offset;

        return flash_hals[plat]->erase(addr);
    }

    uint32_t align(uint32_t val)
    {
        uint32_t plat = hal::platform();
        if (flash_hals[plat] == nullptr)
        {
            return error::NoError;
        }

        return flash_hals[plat]->align(val);
    }

    uint32_t get_sector_size()
    {
        uint32_t plat = hal::platform();
        if (flash_hals[plat] == nullptr)
        {
            return error::NoError;
        }

        return flash_hals[plat]->sector_size();
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

namespace flash_hal_test
{


}

// End of File