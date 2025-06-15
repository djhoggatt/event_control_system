/// @file flash_hal_versatilepb.cpp
/// @author Denver Hoggatt
/// @brief Flash HAL definitions for the versatilepb_qemu board
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "hal.hpp"
#include "flash_hal.hpp"
#include "macros.hpp"

#include <cstdint>
#include <cstdio>

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

    flash_hal::FlashHAL hal_instance;

    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------

    /// @brief Aligns to 8-bytes
    /// @param val Value to align
    /// @return Aligned value
    ///
    uint32_t align_8(uint32_t val)
    {
        return (val + 0x7) & ~0x7;
    }

    // End of Anonymous Namespace
}

namespace flash_hal
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    FlashHAL *versatilepb_qemu_get_funcs()
    {
        return &hal_instance;
    }

    //----------------------------------------------------------------------------------------------
    //  Class Function Definitions
    //----------------------------------------------------------------------------------------------

    error::Error FlashHAL::read(uintptr_t addr, uint8_t *buf, uint32_t size)
    {
        UNUSED(addr);
        UNUSED(buf);
        UNUSED(size);

        error::Error ret_val = error::NoError;

        return ret_val;
    }

    error::Error FlashHAL::write(uintptr_t addr, uint8_t *buf, uint32_t size)
    {
        UNUSED(addr);
        UNUSED(buf);
        UNUSED(size);

        error::Error ret_val = error::NoError;

        return ret_val;
    }

    error::Error FlashHAL::erase(uintptr_t addr)
    {
        UNUSED(addr);

        error::Error ret_val = error::NoError;

        return ret_val;
    }

    uint32_t FlashHAL::align(uint32_t val)
    {
        return align_8(val);
    }

    uint32_t FlashHAL::sector_size()
    {
        return 2048;
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

namespace flash_hal_test
{


}

// End of File