/// @file command_test.cpp
/// @author Denver Hoggatt
/// @brief Unit tests for the command module.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "command.hpp"
#include "control.hpp"
#include "macros.hpp"
#include "input.hpp"
#include "output.hpp"
#include "mem_hal.hpp"
#include "settings.hpp"
#include "flash_hal.hpp"
#include "power_hal.hpp"
#include "fff.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

//--------------------------------------------------------------------------------------------------
//  Private Constants
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  File Variables
//--------------------------------------------------------------------------------------------------

uintptr_t input_ret_val = 0;
void     *out_data;

//--------------------------------------------------------------------------------------------------
//  Fakes/Mocks
//--------------------------------------------------------------------------------------------------

DEFINE_FFF_GLOBALS;

namespace control
{
    FAKE_VALUE_FUNC(char *, get_list_of_controls);
    FAKE_VALUE_FUNC(Control *, get_control_by_name, const char *);

    HandleStatus TestControl1::handle_event(event::Event evt)
    {
        UNUSED(evt);

        return HandleStatus::NotHandled;
    }

    void TestControl1::init_control()
    {
    }

    int32_t Control::get_param(settings::ID setting, uintptr_t value)
    {
        UNUSED(setting);
        UNUSED(value);

        return (int32_t)error::UnknownType;
    }

    int32_t Control::set_param(settings::ID setting, uintptr_t value, bool bootup)
    {
        UNUSED(setting);
        UNUSED(value);
        UNUSED(bootup);

        return (int32_t)error::UnknownType;
    }
}

namespace io
{
    FAKE_VALUE_FUNC(IO *, get_by_id, io::IOID);
    FAKE_VALUE_FUNC(IO *, get_by_name, const char *);
}

namespace input
{
    FAKE_VALUE_FUNC(Input *, get_by_id, io::IOID);
    FAKE_VALUE_FUNC(Input *, get_by_name, const char *);

    FAKE_VALUE_FUNC(char *, cmd_input_override);
    char *Input::cmd_input()
    {
        return cmd_input_override();
    }
}

namespace output
{
    FAKE_VALUE_FUNC(Output *, get_by_id, io::IOID);
    FAKE_VALUE_FUNC(Output *, get_by_name, const char *);

    FAKE_VOID_FUNC(cmd_output_override, uint32_t, char **);
    void Output::cmd_output(uint32_t argc, char **argv)
    {
        cmd_output_override(argc, argv);
    }
}

struct UniqueType
{
};

class TestInput : public input::Input
{
    public:
        void *get_by_id()
        {
            return (void *)input_ret_val;
        }

        void init()
        {
            this->type = io::IOType::GPIO;
        }

        void print(void *data, io::IODirection dir)
        {
            UNUSED(data);
            UNUSED(dir);
        }

        void init_output_info(const std::type_info *type_id, io::IOType io_type)
        {
            this->output_type = type_id;
            this->type        = io_type;
        }

        void cmd_output(uint32_t argc, char **argv)
        {
            UNUSED(argc);
            UNUSED(argv);
        }
};

class TestOutput : public output::Output
{
    public:
        void set_output(void *data)
        {
            out_data = data;
        }

        void print(void *data, io::IODirection dir)
        {
            UNUSED(data);
            UNUSED(dir);
        }

        void init()
        {
            this->type = io::IOType::GPIO;
        }

        void init_input_info(const std::type_info *type_id, io::IOType io_type)
        {
            this->input_type = type_id;
            this->type       = io_type;
        }

        char *cmd_input()
        {
            return nullptr;
        }
};

namespace mem_hal
{
    FAKE_VALUE_FUNC(HeapInfo, get_heap_info);
    FAKE_VALUE_FUNC(uint8_t *, get_stack_pointer);
}

namespace task
{
    FAKE_VOID_FUNC(print_maximum_stack_usage, bool);
}

namespace settings
{
    FAKE_VALUE_FUNC(int32_t, set, settings::ID, const char *, bool);
    FAKE_VALUE_FUNC(int32_t, get, settings::ID, char *);
}

namespace flash_hal
{
    FAKE_VALUE_FUNC(error::Error, read, uint32_t, uint8_t *, uint32_t);
    FAKE_VALUE_FUNC(error::Error, write, uint32_t, uint8_t *, uint32_t);
    FAKE_VALUE_FUNC(error::Error, erase, uint32_t);
}

namespace power_hal
{
    FAKE_VOID_FUNC(reset);
}

//--------------------------------------------------------------------------------------------------
//  Private Functions
//--------------------------------------------------------------------------------------------------

command::CommandFunc get_func(const char *name)
{
    uint32_t              list_size = 0;
    command::CommandFunc *func_list = command::get_func_list(&list_size);
    const char          **name_list = command::get_name_list(&list_size);

    command::CommandFunc ret_val = nullptr;
    for (uint32_t i = 0; i < list_size; i++)
    {
        if (strncmp(name_list[i], name, strlen(name)) == 0)
        {
            ret_val = func_list[i];
            break;
        }
    }

    return ret_val;
}

//--------------------------------------------------------------------------------------------------
//  Tests
//--------------------------------------------------------------------------------------------------

TEST(CommandTest, GetFuncList)
{
    uint32_t              list_size = 0;
    command::CommandFunc *func_list = command::get_func_list(&list_size);

    ASSERT_TRUE(list_size > 0);
    ASSERT_NE(func_list, nullptr);
}

TEST(CommandTest, TestFuncs)
{
    control::TestControl1 ctrl;

    uint32_t              list_size = 0;
    command::CommandFunc *func_list = command::get_func_list(&list_size);

    char test_str[]                               = "test\r\n";
    control::get_list_of_controls_fake.return_val = (char *)test_str;

    for (uint32_t i = 0; i < list_size; i++)
    {
        char *ret_str = func_list[i](0, nullptr);
        ASSERT_NE(ret_str, nullptr);
    }

    control::get_control_by_name_fake.return_val = &ctrl;

    for (uint32_t i = 0; i < list_size; i++)
    {
        const char *arg_list[]    = { "arg1", "arg2" };
        uint32_t    arg_list_size = sizeof(arg_list) / sizeof(arg_list[0]);
        char       *ret_str       = func_list[i](arg_list_size, (char **)arg_list);

        ASSERT_NE(ret_str, nullptr);
    }

    RESET_FAKE(control::get_control_by_name);
    RESET_FAKE(control::get_list_of_controls);
}

TEST(CommandTest, GetNameList)
{
    uint32_t              func_list_size = 0;
    command::CommandFunc *func_list      = command::get_func_list(&func_list_size);

    uint32_t     name_list_size = 0;
    const char **name_list      = command::get_name_list(&name_list_size);
    UNUSED(name_list);

    ASSERT_TRUE(name_list_size > 0);
    ASSERT_NE(func_list, nullptr);
    ASSERT_EQ(func_list_size, name_list_size);
}

TEST(CommandTest, TestNames)
{
    uint32_t     list_size = 0;
    const char **name_list = command::get_name_list(&list_size);

    for (uint32_t i = 0; i < list_size; i++)
    {
        ASSERT_NE(name_list[i], nullptr);
        ASSERT_NE(strlen(name_list[i]), 0);
    }
}

TEST(CommandTest, TestInputs)
{
    command::CommandFunc func = get_func("io-get");
    ASSERT_NE(func, nullptr);

    const char *arr[]    = { "1" };
    uint32_t    arr_size = 1;

    input::get_by_id_fake.return_val = nullptr;
    char *ret_val                    = func(arr_size, (char **)arr);
    ASSERT_NE(ret_val, nullptr);

    TestInput test_input             = TestInput();
    input::get_by_id_fake.return_val = (input::Input *)(&test_input);
    ret_val                          = func(0, nullptr);
    ASSERT_NE(ret_val, nullptr);

    static float tmp_val = 1.1;

    input_ret_val = (uintptr_t)(&tmp_val);
    test_input.init();
    test_input.input_type = &typeid(float *);
    ret_val               = func(arr_size, (char **)arr);
    ASSERT_NE(ret_val, nullptr);

    input_ret_val         = 0;
    test_input.type       = io::IOType::GPIO;
    test_input.input_type = &typeid(bool);
    ret_val               = func(arr_size, (char **)arr);
    ASSERT_NE(ret_val, nullptr);

    test_input.type       = io::IOType::UART;
    test_input.input_type = &typeid(char *);
    ret_val               = func(arr_size, (char **)arr);
    ASSERT_NE(ret_val, nullptr);

    test_input.type       = io::IOType::GPIO;
    test_input.input_type = &typeid(uint32_t);
    ret_val               = func(arr_size, (char **)arr);
    ASSERT_NE(ret_val, nullptr);

    test_input.type       = io::IOType::GPIO;
    test_input.input_type = &typeid(int32_t);
    ret_val               = func(arr_size, (char **)arr);
    ASSERT_NE(ret_val, nullptr);

    test_input.type       = io::IOType::GPIO;
    test_input.input_type = &typeid(UniqueType);
    ret_val               = func(arr_size, (char **)arr);
    ASSERT_NE(ret_val, nullptr);
}

TEST(CommandTest, TestOutputs)
{
    command::CommandFunc func = get_func("io-set");
    ASSERT_NE(func, nullptr);

    const char *arr[]    = { "1", "1" };
    uint32_t    arr_size = 2;

    output::get_by_id_fake.return_val = nullptr;
    char *ret_val                     = func(arr_size, (char **)arr);
    ASSERT_NE(ret_val, nullptr);

    TestOutput test_output            = TestOutput();
    output::get_by_id_fake.return_val = (output::Output *)(&test_output);
    ret_val                           = func(0, nullptr);
    ASSERT_NE(ret_val, nullptr);

    test_output.init();
    test_output.output_type = &typeid(bool);
    ret_val                 = func(arr_size, (char **)arr);
    ASSERT_NE(ret_val, nullptr);

    test_output.type        = io::IOType::UART;
    test_output.output_type = &typeid(char *);
    ret_val                 = func(arr_size, (char **)arr);
    ASSERT_NE(ret_val, nullptr);

    test_output.type        = io::IOType::GPIO;
    test_output.output_type = &typeid(uint32_t);
    ret_val                 = func(arr_size, (char **)arr);
    ASSERT_NE(ret_val, nullptr);

    test_output.type        = io::IOType::GPIO;
    test_output.output_type = &typeid(int32_t);
    ret_val                 = func(arr_size, (char **)arr);
    ASSERT_NE(ret_val, nullptr);

    test_output.type        = io::IOType::GPIO;
    test_output.output_type = &typeid(UniqueType);
    ret_val                 = func(arr_size, (char **)arr);
    ASSERT_NE(ret_val, nullptr);
}

TEST(CommandTest, TestIOList)
{
    command::CommandFunc func = get_func("io-list");
    ASSERT_NE(func, nullptr);

    input::get_by_id_fake.return_val  = nullptr;
    output::get_by_id_fake.return_val = nullptr;

    char *ret_val = func(0, nullptr);
    ASSERT_NE(ret_val, nullptr);

    const char *test_name             = "input1"; // Must fit within TOTAL_IO_NAME_SIZE
    TestOutput  test_output           = TestOutput();
    test_output.id                    = io::IOID::INPUT_1;
    test_output.name                  = test_name;
    output::get_by_id_fake.return_val = (output::Output *)(&test_output);

    ret_val = func(0, nullptr);
    ASSERT_NE(ret_val, nullptr);

    TestInput test_input             = TestInput();
    test_input.id                    = io::IOID::INPUT_1;
    test_input.name                  = test_name;
    input::get_by_id_fake.return_val = (input::Input *)(&test_input);

    ret_val = func(0, nullptr);
    ASSERT_NE(ret_val, nullptr);
}

TEST(CommandTest, IOPrint)
{
    command::CommandFunc func        = get_func("io-print");
    TestOutput           test_output = TestOutput();

    io::get_by_id_fake.return_val = &test_output;

    char *ret_val = func(0, nullptr);
    ASSERT_NE(ret_val, nullptr);
    ASSERT_EQ(test_output.print_io, true);

    func = get_func("io-quiet");

    ret_val = func(0, nullptr);
    ASSERT_NE(ret_val, nullptr);
    ASSERT_EQ(test_output.print_io, false);
}

// End of File