/// @file control_CLI_test.cpp
/// @author Denver Hoggatt
/// @brief Unit tests for the control_CLI module.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "control.hpp"
#include "control_test.hpp"
#include "command.hpp"
#include "io.hpp"
#include "uart.hpp"
#include "event.hpp"
#include "macros.hpp"
#include "fff.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

//--------------------------------------------------------------------------------------------------
//  Private Constants
//--------------------------------------------------------------------------------------------------

constexpr char NEWLINE[] = "\n";
constexpr char PROMPT[]  = ">";
constexpr char HEADER[]  = "Starting Hypercraft CLI:";

//--------------------------------------------------------------------------------------------------
//  File Variables
//--------------------------------------------------------------------------------------------------

static uart::UART  console;
static char       *rcvd_data = nullptr;
static const char *send_data = nullptr;
int32_t            cli_err   = 0;

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
    FAKE_VALUE_FUNC(output::Output *, get_by_id, io::IOID);

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

namespace command
{
    FAKE_VALUE_FUNC(char *, help_func, uint32_t, char **);
    FAKE_VALUE_FUNC(char *, tab_one, uint32_t, char **);
    FAKE_VALUE_FUNC(char *, tab_two, uint32_t, char **);

    const char **get_name_list(uint32_t *size)
    {
        static const char *cmd_name_list[] = { "help", "tab-one", "tab-two" };

        *size = sizeof(cmd_name_list) / sizeof(cmd_name_list[0]);

        return cmd_name_list;
    }

    CommandFunc *get_func_list(uint32_t *size)
    {
        static CommandFunc cmd_func_list[] = { help_func, tab_one, tab_two };

        *size = sizeof(cmd_func_list) / sizeof(cmd_func_list[0]);

        return cmd_func_list;
    }

}

namespace event
{
    FAKE_VOID_FUNC(post, ID, void *);
}

namespace uart
{
    void *UART::get_by_id()
    {
        return (void *)send_data;
    }

    void UART::set_output(void *data)
    {
        REQUIRE(data != nullptr, error::InvalidPointer);

        if (rcvd_data != nullptr)
        {
            free(rcvd_data);
        }

        char *str_data = (char *)data;

        rcvd_data = (char *)malloc(strlen(str_data) + 1); // +1 \0
        strcpy(rcvd_data, str_data);
    }

    void UART::init()
    {
    }

    FAKE_VOID_FUNC(print_override, void *, io::IODirection);
    void UART::print(void *data, io::IODirection dir)
    {
        print_override(data, dir);
    }
}

namespace control
{
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

//--------------------------------------------------------------------------------------------------
//  Private Functions
//--------------------------------------------------------------------------------------------------

static control::CLI *init_cli()
{
    static control::CLI cli;
    static bool         inited = false;

    if (!inited)
    {
        console.uart_port   = uart::VirtualPort::UART_CLI;
        console.id          = io::IOID::UART_CONSOLE;
        console.output_type = &typeid(const char *);
        console.input_type  = &typeid(const char *);

        output::get_by_id_fake.return_val = (output::Output *)(&console);

        cli.init_control();

        inited = true;
    }

    return &cli;
}

static void exec_cmd(control::CLI *cli)
{
    event::Event evt;
    evt.arg = nullptr; // Execute command
    evt.id  = event::ID::control_UpdateCLIState;
    cli->handle_event(evt);

    evt.arg = nullptr; // Write prompt
    evt.id  = event::ID::control_UpdateCLIState;
    cli->handle_event(evt);
}

static void send_cmd(control::CLI *cli, const char *cmd_str)
{
    event::Event evt;
    evt.id    = event::ID::control_UARTInput;
    send_data = cmd_str;
    cli->handle_event(evt);

    // Must not include \r, so we can properly test subsequent CR-LF
    if (cmd_str[strlen(cmd_str) - 1] == '\n')
    {
        exec_cmd(cli);
    }
}

//--------------------------------------------------------------------------------------------------
//  Tests
//--------------------------------------------------------------------------------------------------

TEST(ControlCLITest, Init)
{
    control::CLI *cli = init_cli();
    UNUSED(cli);

    ASSERT_TRUE(output::get_by_id_fake.call_count);
    ASSERT_STREQ(rcvd_data, PROMPT);

    free(rcvd_data);
    rcvd_data = nullptr;
}

TEST(ControlCLITest, InvalidState)
{
    control::CLI *cli = init_cli();

    event::Event evt;
    evt.id = event::ID::control_TestEvent;

    control::HandleStatus ret = cli->handle_event(evt);

    ASSERT_EQ(ret, control::HandleStatus::NotHandled);
}

TEST(ControlCLITest, ExecCmd)
{
    control::CLI *cli = init_cli();

    const char cmd_str[] = "help\r";
    const char ret_str[] = "test return\r\n";

    event::Event evt;
    evt.id    = event::ID::control_UARTInput;
    send_data = cmd_str;
    cli->handle_event(evt);

    ASSERT_EQ(event::post_fake.arg0_val, event::ID::control_UpdateCLIState);

    command::help_func_fake.return_val = (char *)ret_str;
    evt.arg                            = nullptr;
    evt.id                             = event::ID::control_UpdateCLIState;
    cli->handle_event(evt);

    ASSERT_TRUE(command::help_func_fake.call_count);
    ASSERT_STREQ(ret_str, rcvd_data);

    evt.arg = nullptr;
    evt.id  = event::ID::control_UpdateCLIState;
    cli->handle_event(evt);

    ASSERT_STREQ(rcvd_data, PROMPT);
}

TEST(ControlCLITest, TrailingSpace)
{
    control::CLI *cli = init_cli();

    command::help_func_fake.call_count = 0;
    command::help_func_fake.return_val = (char *)"test\r\n";
    strcpy(rcvd_data, "");

    send_cmd(cli, "help ");
    send_cmd(cli, "\n");

    ASSERT_STREQ(PROMPT, rcvd_data);
    ASSERT_TRUE(command::help_func_fake.call_count);
}

TEST(ControlCLITest, Arguments)
{
    control::CLI *cli = init_cli();

    command::help_func_fake.call_count = 0;
    command::help_func_fake.return_val = (char *)"test\r\n";
    strcpy(rcvd_data, "");

    send_cmd(cli, "help arg");
    send_cmd(cli, "\n");

    ASSERT_STREQ(PROMPT, rcvd_data);
    ASSERT_TRUE(command::help_func_fake.call_count);
}

TEST(ControlCLITest, ArgumentTrailingSpace)
{
    control::CLI *cli = init_cli();

    command::help_func_fake.call_count = 0;
    command::help_func_fake.return_val = (char *)"test\r\n";
    strcpy(rcvd_data, "");

    send_cmd(cli, "help arg ");
    send_cmd(cli, "\n");

    ASSERT_STREQ(PROMPT, rcvd_data);
    ASSERT_TRUE(command::help_func_fake.call_count);
}

TEST(ControlCLITest, Backspace)
{
    control::CLI *cli = init_cli();

    command::help_func_fake.call_count = 0;
    command::help_func_fake.return_val = (char *)"test\r\n";
    strcpy(rcvd_data, "");

    send_cmd(cli, "help");

    char cmd_str[] = "\x7F";

    event::Event evt;
    evt.id    = event::ID::control_UARTInput;
    send_data = cmd_str;
    cli->handle_event(evt);

    send_cmd(cli, "\n");

    ASSERT_STREQ(PROMPT, rcvd_data);
    ASSERT_FALSE(command::help_func_fake.call_count);
}

TEST(ControlCLITest, NullBackspace)
{
    control::CLI *cli = init_cli();

    command::help_func_fake.call_count = 0;
    command::help_func_fake.return_val = (char *)"test\r\n";
    strcpy(rcvd_data, "");

    char cmd_str[] = "\x7F";

    event::Event evt;
    evt.id    = event::ID::control_UARTInput;
    send_data = cmd_str;
    cli->handle_event(evt);

    ASSERT_STREQ(PROMPT, rcvd_data);
}

TEST(ControlCLITest, Up)
{
    control::CLI *cli = init_cli();

    command::help_func_fake.call_count = 0;
    command::help_func_fake.return_val = (char *)"test\r\n";
    strcpy(rcvd_data, "");

    send_cmd(cli, "help\n");

    command::help_func_fake.call_count = 0;
    command::help_func_fake.return_val = (char *)"test\r\n";
    strcpy(rcvd_data, "");

    const char up_arrow[] = { 0x1B, 0x5B, 0x41, 0x00 };
    send_cmd(cli, up_arrow);

    send_cmd(cli, "\n");

    ASSERT_STREQ(PROMPT, rcvd_data);
    ASSERT_TRUE(command::help_func_fake.call_count);
}

TEST(ControlCLITest, Down)
{
    control::CLI *cli = init_cli();

    command::help_func_fake.call_count = 0;
    command::help_func_fake.return_val = (char *)"test\r\n";
    strcpy(rcvd_data, "");

    send_cmd(cli, "help");

    const char down_arrow[] = { 0x1B, 0x5B, 0x42, 0x00 };
    send_cmd(cli, down_arrow);

    exec_cmd(cli);

    send_cmd(cli, "\n");

    ASSERT_STREQ(PROMPT, rcvd_data);
    ASSERT_FALSE(command::help_func_fake.call_count);
}

TEST(ControlCLITest, TabList)
{
    control::CLI *cli = init_cli();

    command::tab_one_fake.call_count = 0;
    command::tab_two_fake.call_count = 0;

    command::tab_one_fake.return_val = (char *)"test\r\n";
    command::tab_two_fake.return_val = (char *)"test\r\n";

    strcpy(rcvd_data, "");

    send_cmd(cli, "tab\t");
    send_cmd(cli, "\n");

    ASSERT_STREQ(PROMPT, rcvd_data);
    ASSERT_FALSE(command::tab_one_fake.call_count);
    ASSERT_FALSE(command::tab_two_fake.call_count);
}

TEST(ControlCLITest, TabComplete)
{
    control::CLI *cli = init_cli();

    command::tab_one_fake.call_count = 0;
    command::tab_one_fake.return_val = (char *)"test\r\n";
    strcpy(rcvd_data, "");

    send_cmd(cli, "tab-o\t");
    send_cmd(cli, "\n");

    ASSERT_STREQ(PROMPT, rcvd_data);
    ASSERT_TRUE(command::tab_one_fake.call_count);
    ASSERT_FALSE(command::tab_two_fake.call_count);
}

TEST(ControlCLITest, TabCompleteEmpty)
{
    control::CLI *cli = init_cli();

    command::tab_one_fake.call_count = 0;
    command::tab_one_fake.return_val = (char *)"test\r\n";
    strcpy(rcvd_data, "");

    send_cmd(cli, "\n");
    send_cmd(cli, "\t");
    send_cmd(cli, "\n");
}

TEST(ControlCLITest, CRLF)
{
    control::CLI *cli = init_cli();

    RESET_FAKE(event::post);
    strcpy(rcvd_data, "");

    send_cmd(cli, "\r");
    send_cmd(cli, "\n");

    ASSERT_STREQ(PROMPT, rcvd_data);
    ASSERT_TRUE(event::post_fake.call_count > 2);
}

TEST(ControlCLITest, TooLong)
{
    control::CLI *cli = init_cli();

    uint32_t cmd_len = control::CMD_STR_LEN;

    char str[cmd_len * 2];

    memset(str, 'a', cmd_len * 2);
    str[(cmd_len * 2) - 1] = '\0';

    send_cmd(cli, (const char *)str);
}

TEST(ControlCLITest, TooManySpaces)
{
    control::CLI *cli = init_cli();

    uint32_t cmd_len = control::CMD_STR_LEN - 1;

    char str[cmd_len];
    memset(str, ' ', cmd_len);

    str[0]           = 'h';
    str[1]           = 'e';
    str[2]           = 'l';
    str[3]           = 'p';
    str[cmd_len - 2] = '\n';
    str[cmd_len - 1] = '\0';

    send_cmd(cli, (const char *)str);
}

TEST(ControlCLITest, MaxArgs)
{
    control::CLI *cli = init_cli();

    uint32_t cmd_len = control::CMD_STR_LEN - 1;

    char str[cmd_len];

    for (uint32_t i = 0; i < (cmd_len - 1); i++)
    {
        str[i] = (i % 2) == 0 ? 'a' : ' ';
    }

    str[cmd_len - 2] = '\n';
    str[cmd_len - 1] = '\0';

    send_cmd(cli, (const char *)str);
}

TEST(ControlCLITest, CLIOutput)
{
    control::CLI *cli = init_cli();

    const char str[] = "test\r\n";

    event::Event evt;
    evt.id  = event::ID::control_CLIOutput;
    evt.arg = (void *)str;
    cli->handle_event(evt);

    ASSERT_STREQ(str, rcvd_data);
}

TEST(ControlCLITest, GetSetParam)
{
    control::CLI *cli = init_cli();

    error::Error err
        = (error::Error)cli->set_param(settings::ID::ErrorHandleType, (uintptr_t)cli_err, false);

    ASSERT_EQ(err, error::UnknownType);

    err = (error::Error)cli->get_param(settings::ID::ErrorHandleType, (uintptr_t)&cli_err);

    ASSERT_EQ(err, error::UnknownType);
}

// End of File