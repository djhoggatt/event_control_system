/// @file adc_test.cpp
/// @author Denver Hoggatt
/// @brief Unit tests for the adc module.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "adc.hpp"
#include "adc_test.hpp"
#include "error.hpp"
#include "event.hpp"
#include "periodic.hpp"
#include "macros.hpp"
#include "fff.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

//--------------------------------------------------------------------------------------------------
//  Private Constants
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  File Variables
//--------------------------------------------------------------------------------------------------

constexpr uint16_t TEST_VAL        = 1365;
constexpr float    TEST_VAL_CONV   = 1.1;
constexpr uint16_t TEST_VAL_2      = 2730;
constexpr float    TEST_VAL_CONV_2 = 2.2;

uint16_t read_val;

//--------------------------------------------------------------------------------------------------
//  Fakes/Mocks
//--------------------------------------------------------------------------------------------------

DEFINE_FFF_GLOBALS;

namespace io
{
    FAKE_VOID_FUNC(print_override, const char *, const char *, IOID, char *, IODirection);
    void print(const char *io, const char *name, IOID id, char *data, IODirection dir)
    {
        print_override(io, name, id, data, dir);
    }
}

namespace input
{
    FAKE_VOID_FUNC(init_input_info_override, const std::type_info *, io::IOType);
    void Input::init_input_info(const std::type_info *type_id, io::IOType io_type)
    {
        this->input_type = type_id;
        this->type       = io_type;
        this->direction  = io::IODirection::input;
        init_input_info_override(type_id, io_type);
    }

    FAKE_VALUE_FUNC(char *, cmd_input_override);
    char *Input::cmd_input()
    {
        return cmd_input_override();
    }
}

namespace adc_hal
{
    FAKE_VALUE_FUNC(error::Error, open);
    FAKE_VALUE_FUNC(uint32_t, get_bit_width, adc::VirtualPort);
    FAKE_VALUE_FUNC(float, get_ref_voltage);
    FAKE_VOID_FUNC(start_conversions);

    error::Error read(adc::VirtualPort pin, uint16_t *val)
    {
        *val = read_val;

        return error::NoError;
    }
}

namespace event
{
    FAKE_VOID_FUNC(post, ID, void *);
    FAKE_VALUE_FUNC(uint32_t, count_arg, ID, void *);
    FAKE_VALUE_FUNC(uint32_t, count, ID);
}

namespace periodic
{
    FAKE_VOID_FUNC(start, ID);
    FAKE_VOID_FUNC(create, ID, uint32_t, CallbackFunc);
}

//--------------------------------------------------------------------------------------------------
//  Private Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  Tests
//--------------------------------------------------------------------------------------------------

TEST(ADCTest, init)
{
    adc::ADC test_adc_1 = adc::ADC();
    test_adc_1.id       = io::IOID::INPUT_1;
    test_adc_1.init();

    ASSERT_EQ(test_adc_1.type, io::IOType::ADC);
    ASSERT_EQ(test_adc_1.id, io::IOID::INPUT_1);
    ASSERT_EQ(test_adc_1.direction, io::IODirection::input);

    adc::ADC test_adc_2 = adc::ADC();
    test_adc_2.id       = io::IOID::INPUT_2;
    test_adc_2.init();

    ASSERT_EQ(test_adc_2.type, io::IOType::ADC);
    ASSERT_EQ(test_adc_2.id, io::IOID::INPUT_2);
    ASSERT_EQ(test_adc_2.direction, io::IODirection::input);
}

TEST(ADCTest, ISRRead)
{
    adc::isr_adc(adc::VirtualPort::NumPorts);

    ASSERT_FALSE(event::post_fake.call_count);

    adc::isr_adc(adc::VirtualPort::ADC_1);

    ASSERT_TRUE(event::post_fake.call_count);
}

TEST(ADCTest, GetData)
{
    adc_hal::get_bit_width_fake.return_val   = 12;
    adc_hal::get_ref_voltage_fake.return_val = 3.3;

    adc::ADC test_adc_3 = adc::ADC();
    test_adc_3.id       = io::IOID::INPUT_1;
    test_adc_3.adc_port = adc::VirtualPort::ADC_1;
    test_adc_3.print_io = true;
    test_adc_3.init();

    read_val = TEST_VAL;

    test_adc_3.input_type = &typeid(float *);
    ASSERT_EQ(*test_adc_3.get<float *>(), TEST_VAL_CONV);

    // Repeat to test that index is properly incrementing
    adc::ADC test_adc_4 = adc::ADC();
    test_adc_4.id       = io::IOID::INPUT_2;
    test_adc_4.adc_port = adc::VirtualPort::ADC_2;
    test_adc_4.init();

    read_val = TEST_VAL_2;

    test_adc_4.input_type = &typeid(float *);
    ASSERT_EQ(*test_adc_4.get<float *>(), TEST_VAL_CONV_2);
}

TEST(ADCTest, StartConversion)
{
    adc::ADC test_adc_1 = adc::ADC();
    test_adc_1.id       = io::IOID::INPUT_1;
    test_adc_1.init();

    periodic::CallbackFunc func  = periodic::create_fake.arg2_history[0];
    event::count_fake.return_val = 1;
    func(0);

    ASSERT_TRUE(adc_hal::start_conversions_fake.call_count);
}

// End of File