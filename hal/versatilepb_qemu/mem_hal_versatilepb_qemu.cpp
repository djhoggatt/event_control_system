/// @file mem_hal_versatilepb_qemu.cpp
/// @author Denver Hoggatt
/// @brief Memory HAL for the versatilepb_qeumu
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "mem_hal.hpp"

#include <cstdint>

extern char _heap_start; // Defined in linker script
extern char _heap_end;   // Defined in linker script

void *_heap_max;

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

    mem_hal::MemHAL hal_instance;

    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------


    // End of Anonymous Namespace
}

namespace mem_hal
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    MemHAL *versatilepb_qemu_get_funcs()
    {
        return &hal_instance;
    }

    //----------------------------------------------------------------------------------------------
    //  Class Function Definitions
    //----------------------------------------------------------------------------------------------

    HeapInfo MemHAL::get_heap_info()
    {
        HeapInfo ret_val;

        ret_val.base = ((uint8_t *)&_heap_start);
        ret_val.end  = ((uint8_t *)&_heap_end);
        ret_val.max  = ((uint8_t *)_heap_max);

        return ret_val;
    }

    uint8_t *MemHAL::get_stack_pointer()
    {
        uintptr_t stack_pointer = (uintptr_t)nullptr;

        __asm volatile("mov %0, sp" : "=r"(stack_pointer));

        return (uint8_t *)stack_pointer;
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

namespace mem_hal_test
{


}

// End of File