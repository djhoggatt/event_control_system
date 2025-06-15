/// @file gpio_test.cpp
/// @author Denver Hoggatt
/// @brief Unit tests for the gpio module.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "gpio_hal.hpp"
#include "error.hpp"
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

bool init_input_called;
bool init_output_called;

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
        init_input_info_override(type_id, io_type);
    }

    FAKE_VALUE_FUNC(char *, cmd_input_override);
    char *Input::cmd_input()
    {
        return cmd_input_override();
    }
}

namespace output
{
    FAKE_VOID_FUNC(init_output_info_override, const std::type_info *, io::IOType);
    void Output::init_output_info(const std::type_info *type_id, io::IOType io_type)
    {
        this->output_type = type_id;
        this->type        = io_type;
        init_output_info_override(type_id, io_type);
    }

    FAKE_VOID_FUNC(cmd_output_override, uint32_t, char **);
    void Output::cmd_output(uint32_t argc, char **argv)
    {
        cmd_output_override(argc, argv);
    }
}

namespace gpio_hal
{
    FAKE_VALUE_FUNC(bool, read, gpio::VirtualPort);
    FAKE_VALUE_FUNC(error::Error, set, gpio::VirtualPort);
    FAKE_VALUE_FUNC(error::Error, reset, gpio::VirtualPort);
    FAKE_VALUE_FUNC(error::Error, open);
}

//--------------------------------------------------------------------------------------------------
//  Private Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  Tests
//--------------------------------------------------------------------------------------------------

TEST(GPIOTest, InputInit)
{
    gpio::GPIO test_gpio = gpio::GPIO();

    test_gpio.init();

    ASSERT_TRUE(input::init_input_info_override_fake.call_count);
}

TEST(GPIOTest, OutputInit)
{
    gpio::GPIO test_gpio = gpio::GPIO();

    test_gpio.init();

    ASSERT_TRUE(output::init_output_info_override_fake.call_count);
}

TEST(GPIOTest, OutputData)
{
    gpio::GPIO test_gpio = gpio::GPIO();
    test_gpio.init();

    test_gpio.print_io = true;
    test_gpio.set<bool>(true);
    ASSERT_EQ(gpio_hal::set_fake.call_count, 1);
    ASSERT_EQ(gpio_hal::reset_fake.call_count, 0);

    RESET_FAKE(gpio_hal::set);
    RESET_FAKE(gpio_hal::reset);

    test_gpio.set<bool>(false);
    ASSERT_EQ(gpio_hal::set_fake.call_count, 0);
    ASSERT_EQ(gpio_hal::reset_fake.call_count, 1);
}

TEST(GPIOTest, OutputDataPostCond)
{
    gpio::GPIO test_gpio = gpio::GPIO();
    test_gpio.init();

    gpio_hal::set_fake.return_val = error::InvalidPin;

    TEST_ERROR(test_gpio.set<bool>(true));
}

TEST(GPIOTest, InputData)
{
    gpio::GPIO test_gpio = gpio::GPIO();
    test_gpio.init();

    gpio_hal::read_fake.return_val = false;
    ASSERT_FALSE(test_gpio.get<bool>());

    gpio_hal::read_fake.return_val = true;
    ASSERT_TRUE(test_gpio.get<bool>());
}

// End of File