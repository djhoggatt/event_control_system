/// @file adc_hal.cpp
/// @author Denver Hoggatt
/// @brief ADC HAL definitions
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "hal.hpp"
#include "adc_hal.hpp"
#include "error.hpp"
#include "macros.hpp"

#include <cstdint>
#include <cstring>

#define INCLUDE_PLATFORM_HEADERS 1
#include "platforms.def"

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------

namespace
{
    //----------------------------------------------------------------------------------------------
    //  Private Constants
    //----------------------------------------------------------------------------------------------

    constexpr uint16_t DEFAULT_BIT_WIDTH = 12; // Prevents div-by-0

    //----------------------------------------------------------------------------------------------
    //  Private Data Types
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Private Function Prototypes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  File Variables
    //----------------------------------------------------------------------------------------------

    uint32_t adc_pins[(uint32_t)hal::Platform::NumPlatforms][(uint32_t)adc::VirtualPort::NumPorts];
    uint32_t adc_ports[(uint32_t)hal::Platform::NumPlatforms][(uint32_t)adc::VirtualPort::NumPorts];
    adc_hal::ADCHAL *adc_funcs[(uint32_t)hal::Platform::NumPlatforms];

    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------


    // End of Anonymous Namespace
}

namespace adc_hal
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    /// @brief Starts ADC conversions
    ///
    void start_conversions()
    {
        if (adc_funcs[hal::platform()] == nullptr)
        {
            return;
        }

        adc_funcs[hal::platform()]->start_conversion();
    }

    /// @brief Gets the ADC reference voltage
    /// @return Reference voltage
    ///
    float get_ref_voltage()
    {
        if (adc_funcs[hal::platform()] == nullptr)
        {
            return 0.0;
        }

        return adc_funcs[hal::platform()]->get_ref_voltage();
    }

    /// @brief Gets the ADC bit-width.
    /// @param pin Virtual port to get.
    /// @return Bit width of the ADC.
    ///
    uint32_t get_bit_width(adc::VirtualPort pin)
    {
        UNUSED(pin);

        if (adc_funcs[hal::platform()] == nullptr)
        {
            return 0;
        }

        uint32_t ret_val = 0;

        uint32_t plat = hal::platform();
        if (adc_pins[plat][(uint32_t)pin] == UINT_MAX)
        {
            ret_val = DEFAULT_BIT_WIDTH;
        }
        else
        {
            ret_val = adc_funcs[plat]->get_bit_width(adc_ports[plat][(uint32_t)pin],
                                                     adc_pins[plat][(uint32_t)pin]);
        }

        return ret_val;
    }

    /// @brief Reads the value from the ADC.
    /// @param pin Virtual port for the ADC.
    /// @param val Pointer to the value that will be set.
    /// @return No error on success.
    ///
    error::Error read(adc::VirtualPort pin, uint16_t *val)
    {
        if (pin >= adc::VirtualPort::NumPorts)
        {
            return error::InvalidPin;
        }

        uint32_t plat = hal::platform();
        if (adc_pins[plat][(uint32_t)pin] == UINT_MAX)
        {
            return error::NoError;
        }

        if (adc_funcs[plat] == nullptr)
        {
            return error::NoError;
        }

        uint32_t uint_pin = (uint32_t)pin;

        return adc_funcs[plat]->read(adc_ports[plat][uint_pin], adc_pins[plat][uint_pin], val);
    }

    /// @brief Initializes the ADC HAL.
    ///
    void init()
    {
#define DEF_PLAT(plat_name) adc_funcs[(uint32_t)hal::Platform::plat_name] = plat_name##_get_funcs();
#include "platforms.def"
#undef DEF_PLAT

        memset(adc_pins, 0xFF, sizeof(adc_pins));
        memset(adc_ports, 0xFF, sizeof(adc_pins));

#define DEF_ADC(PLAT, VIRT_PIN, PHY_PIN, PHY_PORT, ACTIVE, HANDLE) \
    adc_pins[(uint32_t)hal::Platform::PLAT][(uint32_t)adc::VirtualPort::VIRT_PIN] = PHY_PIN;
#include "platforms.def"
#undef DEF_ADC

#define DEF_ADC(PLAT, VIRT_PIN, PHY_PIN, PHY_PORT, ACTIVE, HANDLE) \
    adc_ports[(uint32_t)hal::Platform::PLAT][(uint32_t)adc::VirtualPort::VIRT_PIN] = PHY_PORT;
#include "platforms.def"
#undef DEF_ADC

        error::Error err = error::NoError;
        if (adc_funcs[hal::platform()] != nullptr)
        {
            err = adc_funcs[hal::platform()]->open();
        }

        ENSURE(err == error::NoError, error::DeviceInitFailed);
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

namespace adc_hal_test
{


}

// End of File