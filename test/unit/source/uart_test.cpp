/// @file uart_test.cpp
/// @author Denver Hoggatt
/// @brief Unit tests for the uart module.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "uart.hpp"
#include "event.hpp"
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
        UNUSED(type_id);
        this->input_type = &typeid(const char *);
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
        UNUSED(type_id);
        this->output_type = &typeid(const char *);
        this->type        = io_type;
        init_output_info_override(type_id, io_type);
    }

    FAKE_VOID_FUNC(cmd_output_override, uint32_t, char **);
    void Output::cmd_output(uint32_t argc, char **argv)
    {
        cmd_output_override(argc, argv);
    }
}

namespace uart_hal
{
    FAKE_VALUE_FUNC(error::Error, open, uart::VirtualPort);
    FAKE_VALUE_FUNC(error::Error, send, uart::VirtualPort, const char *);
}

namespace event
{
    FAKE_VOID_FUNC(post, ID, void *);
    FAKE_VALUE_FUNC(uint32_t, count, ID);
}

//--------------------------------------------------------------------------------------------------
//  Private Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  Tests
//--------------------------------------------------------------------------------------------------

TEST(TaskUARTTest, Constructor)
{
    uart_hal::open_fake.return_val = error::NoError;

    uart::UART uart = uart::UART();
    uart.type       = io::IOType::UART;
    uart.id         = io::IOID::UART_CONSOLE;
    uart.direction  = io::IODirection::input_output;

    ASSERT_EQ(uart.type, io::IOType::UART);
    ASSERT_EQ(uart.id, io::IOID::UART_CONSOLE);
    ASSERT_EQ(uart.direction, io::IODirection::input_output);
}

TEST(TaskUARTTest, SetOutput)
{
    uart_hal::open_fake.return_val = error::NoError;
    uart::UART uart                = uart::UART();
    uart.init();
    uart.print_io = true;

    char str[32] = "Test String";
    uart.set<const char *>(str);

    ASSERT_STREQ(str, uart_hal::send_fake.arg1_val);
}

TEST(TaskUARTTest, GetData)
{
    uart_hal::open_fake.return_val = error::NoError;
    uart::UART uart                = uart::UART();
    uart.uart_port                 = uart::VirtualPort::UART_CLI;
    uart.init();

    char test_str[] = "Test";

    uart::isr_read(test_str[0]);
    uart::isr_read(test_str[1]);
    uart::isr_read(test_str[2]);
    uart::isr_read(test_str[3]);

    const char *data = uart.get<const char *>();

    ASSERT_STREQ(data, test_str);
}

TEST(TaskUARTTest, ReadISR)
{
    uart::UART uart = uart::UART();
    uart.uart_port  = uart::VirtualPort::UART_CLI;
    uart.init();

    char test_char = 'T';
    uart::isr_read(test_char);

    ASSERT_TRUE(event::post_fake.call_count);
}

// End of File