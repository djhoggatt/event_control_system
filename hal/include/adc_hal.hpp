/// @file adc_hal.hpp
/// @author Denver Hoggatt
/// @brief ADC HAL declarations
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
#include "adc.hpp"

#include <cstdint>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------


namespace adc_hal
{
    //----------------------------------------------------------------------------------------------
    //  Public Constants
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Data Types
    //----------------------------------------------------------------------------------------------

    class ADCHAL : public hal::HAL
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

            error::Error open();

            error::Error read(uint32_t port, uint32_t pin, uint16_t *val);

            uint32_t get_bit_width(uint32_t port, uint32_t pin);

            float get_ref_voltage();

            void start_conversion();

            // End of Class
    };

    //----------------------------------------------------------------------------------------------
    //  Classes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    void start_conversions();

    error::Error read(adc::VirtualPort pin, uint16_t *val);

    uint32_t get_bit_width(adc::VirtualPort pin);

    float get_ref_voltage();

    void init();

#define DEF_PLAT(plat_name) ADCHAL *plat_name##_get_funcs();
#include "platforms.def"
#undef DEF_PLAT

    // End of Namespace
}

// End of File