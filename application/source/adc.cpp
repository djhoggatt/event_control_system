/// @file adc.cpp
/// @author Denver Hoggatt
/// @brief ADC definitions
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "adc.hpp"
#include "adc_hal.hpp"
#include "input.hpp"
#include "error.hpp"
#include "event.hpp"
#include "io.hpp"
#include "periodic.hpp"
#include "macros.hpp"

#include <cinttypes>
#include <cstring>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------

namespace
{
    //----------------------------------------------------------------------------------------------
    //  Private Constants
    //----------------------------------------------------------------------------------------------

    constexpr uint32_t CONVERSION_INTERVAL = 1;

    //----------------------------------------------------------------------------------------------
    //  Private Data Types
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Private Function Prototypes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  File Variables
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------

    /// @brief Starts the conversions for non-continuous conversions.
    /// @param curr_time_ms Current system time.
    ///
    void start_conversion(uint32_t curr_time_ms)
    {
        UNUSED(curr_time_ms);

        adc_hal::start_conversions();
    }

    // End of Anonymous Namespace
}

namespace adc
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    /// @brief ADC handling ISR. Should only be called in an ISR context.
    /// @param port Virtual port of the calling ADC.
    ///
    void isr_adc(VirtualPort port)
    {
        if (port >= VirtualPort::NumPorts)
        {
            return;
        }

        event::post(event::ID::control_ADCInput, (void *)(uintptr_t)port);
    }

    //----------------------------------------------------------------------------------------------
    //  Class Function Definitions
    //----------------------------------------------------------------------------------------------

    /// @brief Prints the I/O access.
    /// @param data Data associated with the access.
    /// @param dir Direction of the access.
    ///
    void ADC::print(void *data, io::IODirection dir)
    {
        // sign + digits + dot + fractional + null
        constexpr uint32_t NUM_DIGITS = 1 + 39 + 1 + 6 + 1;

        static char data_str[NUM_DIGITS + 1]; // +1 \0
        sprintf(data_str, "%f", (double)(*(float *)data));

        io::print("ADC", this->name, this->id, data_str, dir);
    }

    /// @brief Gets the input data.
    /// @return Input data.
    ///
    void *ADC::get_by_id()
    {
        uint16_t val = 0;
        adc_hal::read(this->adc_port, &val);

        uint32_t max_read_val = (1 << adc_hal::get_bit_width(this->adc_port)) - 1;
        float    scale        = adc_hal::get_ref_voltage() / ((float)max_read_val);

        this->read_val = scale * ((float)val);

        return (void *)(&this->read_val);
    }

    /// @brief Initializes the IO.
    ///
    void ADC::init()
    {
        REENTRY_GUARD_CLASS();

        this->init_input_info(&typeid(float *), io::IOType::ADC);

        periodic::create(periodic::ID::ADCConversion, CONVERSION_INTERVAL, start_conversion);

        periodic::start(periodic::ID::ADCConversion);
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

namespace adc_test
{


}

// End of File