/// @file io_test.cpp
/// @author Denver Hoggatt
/// @brief Unit tests for the io module.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "io.hpp"
#include "input.hpp"
#include "output.hpp"
#include "macros.hpp"
#include "adc_test.hpp"
#include "adc.hpp"
#include "uart.hpp"
#include "gpio.hpp"
#include "fff.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>

//--------------------------------------------------------------------------------------------------
//  Private Constants
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  File Variables
//--------------------------------------------------------------------------------------------------

bool set_inputs  = false;
bool set_outputs = false;
bool adc_inited  = false;

//--------------------------------------------------------------------------------------------------
//  Fakes/Mocks
//--------------------------------------------------------------------------------------------------

DEFINE_FFF_GLOBALS;

namespace input
{
    FAKE_VOID_FUNC(init_input_info_override, const std::type_info *, io::IOType);
    void Input::init_input_info(const std::type_info *type_id, io::IOType io_type)
    {
        this->input_type = type_id;
        this->type       = io_type;

        if (this->direction == io::IODirection::output)
        {
            this->direction = io::IODirection::input_output;
        }
        else if (this->direction != io::IODirection::input_output)
        {
            this->direction = io::IODirection::input;
        }

        init_input_info_override(type_id, io_type);
    }

    FAKE_VALUE_FUNC(char *, cmd_input_override);
    char *Input::cmd_input()
    {
        return cmd_input_override();
    }

    void init_input_list(input::Input **list, uint32_t size)
    {
        EXPECT_TRUE(list != nullptr);
        EXPECT_TRUE(size != 0);

        set_inputs = true;
    }

    FAKE_VALUE_FUNC(Input *, get_by_id, io::IOID);
    FAKE_VALUE_FUNC(Input *, get_by_name, const char *);
}

namespace output
{
    FAKE_VOID_FUNC(init_output_info_override, const std::type_info *, io::IOType);
    void Output::init_output_info(const std::type_info *type_id, io::IOType io_type)
    {
        this->output_type = type_id;
        this->type        = io_type;

        if (this->direction == io::IODirection::input)
        {
            this->direction = io::IODirection::input_output;
        }
        else if (this->direction != io::IODirection::input_output)
        {
            this->direction = io::IODirection::output;
        }

        init_output_info_override(type_id, io_type);
    }

    FAKE_VOID_FUNC(cmd_output_override, uint32_t, char **);
    void Output::cmd_output(uint32_t argc, char **argv)
    {
        cmd_output_override(argc, argv);
    }

    void init_output_list(Output **list, uint32_t size)
    {
        EXPECT_TRUE(list != nullptr);
        EXPECT_TRUE(size != 0);

        set_outputs = true;
    }

    FAKE_VALUE_FUNC(Output *, get_by_id, io::IOID);
    FAKE_VALUE_FUNC(Output *, get_by_name, const char *);
}

namespace adc
{
    void *ADC::get_by_id()
    {
        return nullptr;
    }

    void ADC::init()
    {
    }

    FAKE_VOID_FUNC(print_override, void *, io::IODirection);
    void ADC::print(void *data, io::IODirection dir)
    {
        print_override(data, dir);
    }
}

namespace uart
{
    void *UART::get_by_id()
    {
        return {};
    }

    void UART::set_output(void *data)
    {
        UNUSED(data);
    }

    void UART::init()
    {
        this->init_input_info(&typeid(const char *), io::IOType::UART);
        this->init_output_info(&typeid(const char *), io::IOType::UART);
    }

    FAKE_VOID_FUNC(print_override, void *, io::IODirection);
    void UART::print(void *data, io::IODirection dir)
    {
        print_override(data, dir);
    }
}

namespace gpio
{
    FAKE_VOID_FUNC(init_override)
    void GPIO::init()
    {
        this->input_type  = &typeid(bool);
        this->output_type = &typeid(bool);
        init_override();
    }

    FAKE_VOID_FUNC(set_output_override, void *);
    void GPIO::set_output(void *data)
    {
        set_output_override(data);
    }

    FAKE_VALUE_FUNC(void *, get_by_id_override);
    void *GPIO::get_by_id()
    {
        return get_by_id_override();
    }

    FAKE_VOID_FUNC(print_override, void *, io::IODirection);
    void GPIO::print(void *data, io::IODirection dir)
    {
        print_override(data, dir);
    }
}

//--------------------------------------------------------------------------------------------------
//  Private Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  Tests
//--------------------------------------------------------------------------------------------------

TEST(IOTest, Open)
{
    io::open();

    ASSERT_TRUE(set_inputs);
    ASSERT_TRUE(set_outputs);
}

TEST(IOTest, GetType)
{
    io::IOType io_type = io::get_type(io::IOID::INPUT_1);

    ASSERT_EQ(io_type, io::IOType::ADC);
}

TEST(IOTest, InitInputInfo)
{
    uart::UART test_uart = uart::UART();

    test_uart.id        = io::IOID::UART_CONSOLE;
    test_uart.direction = io::IODirection::input;

    test_uart.init();

    ASSERT_EQ(test_uart.id, io::IOID::UART_CONSOLE);
    ASSERT_EQ(*(test_uart.input_type), typeid(const char *));
    ASSERT_EQ(test_uart.type, io::IOType::UART);
    ASSERT_EQ(test_uart.direction, io::IODirection::input_output);
}

TEST(IOTest, InitOutputInfo)
{
    uart::UART test_uart = uart::UART();

    test_uart.id        = io::IOID::UART_CONSOLE;
    test_uart.direction = io::IODirection::output;
    test_uart.init();

    ASSERT_EQ(test_uart.id, io::IOID::UART_CONSOLE);
    ASSERT_EQ(*(test_uart.output_type), typeid(const char *));
    ASSERT_EQ(test_uart.type, io::IOType::UART);
    ASSERT_EQ(test_uart.direction, io::IODirection::input_output);
}

TEST(IOTest, GetIONull)
{
    output::get_by_id_fake.return_val = nullptr;
    input::get_by_id_fake.return_val  = nullptr;
    io::IO *test_io                   = io::get_by_id(io::IOID::UART_CONSOLE);
    ASSERT_EQ(test_io, nullptr);

    output::get_by_name_fake.return_val = nullptr;
    input::get_by_name_fake.return_val  = nullptr;
    test_io                             = io::get_by_name("Test Name");
    ASSERT_EQ(test_io, nullptr);
}

TEST(IOTest, GetIO)
{
    uart::UART test_uart = uart::UART();

    output::get_by_id_fake.return_val = &test_uart;
    input::get_by_id_fake.return_val  = &test_uart;
    io::IO *test_io                   = io::get_by_id(io::IOID::UART_CONSOLE);
    ASSERT_EQ(test_io, &test_uart);

    output::get_by_name_fake.return_val = &test_uart;
    input::get_by_name_fake.return_val  = &test_uart;
    test_io                             = io::get_by_name("Test Name");
    ASSERT_EQ(test_io, &test_uart);
}

TEST(IOTest, Print)
{
    char            test_io[]   = "TestIO";
    char            test_name[] = "TestName";
    char            test_data[] = "TestData";
    io::IOID        test_id     = io::IOID::UART_CONSOLE;
    io::IODirection test_dir    = io::IODirection::input;

    io::print(test_io, test_name, test_id, test_data, test_dir);

    test_dir = io::IODirection::output;

    io::print(test_io, test_name, test_id, test_data, test_dir);
}

// End of File