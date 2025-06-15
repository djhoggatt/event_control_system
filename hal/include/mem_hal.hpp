
/// @file isr_hal.hpp
/// @author Denver Hoggatt
/// @brief Memory HAL declarations
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#pragma once

#include "hal.hpp"

#include <cstdint>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------


namespace mem_hal
{
    //----------------------------------------------------------------------------------------------
    //  Public Constants
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Data Types
    //----------------------------------------------------------------------------------------------

    struct HeapInfo
    {
            uint8_t *base;
            uint8_t *end;
            uint8_t *max;
    };

    //----------------------------------------------------------------------------------------------
    //  Classes
    //----------------------------------------------------------------------------------------------

    class MemHAL : public hal::HAL
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

            HeapInfo get_heap_info();

            uint8_t *get_stack_pointer();

            // End of Class
    };

    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    HeapInfo get_heap_info();

    uint8_t *get_stack_pointer();

    void init();

#define DEF_PLAT(plat_name) MemHAL *plat_name##_get_funcs();
#include "platforms.def"
#undef DEF_PLAT

    // End of Namespace
}

// End of File