/// @file control_CLI.cpp
/// @author Denver Hoggatt
/// @brief CLI implementation
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "control.hpp"
#include "uart.hpp"
#include "command.hpp"
#include "event.hpp"
#include "macros.hpp"

#include <cstdint>
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

    constexpr bool ECHO_INPUT = true;

    const char ECHO_EXCEPTIONS[] = { '\t' };

    //----------------------------------------------------------------------------------------------
    //  Private Data Types
    //----------------------------------------------------------------------------------------------

    enum class CLIState : uint32_t
    {
        WritingPrompt,
        WaitingForInput,
        ExecutingCommand,
    };

    //----------------------------------------------------------------------------------------------
    //  Private Function Prototypes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  File Variables
    //----------------------------------------------------------------------------------------------

    CLIState    current_state;
    uart::UART *console;

    char     current_cmd[control::CMD_STR_LEN + 1]; // +1 for \0
    char     last_cmd[control::CMD_STR_LEN + 1];
    uint32_t current_position = 0;

    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------

    /// @brief CLI Start line
    ///
    void write_header()
    {
        console->set<const char *>("Starting Command Line Interface:");
    }

    /// @brief Newline sequence
    ///
    void write_newline()
    {
        fflush(stdout);
        console->set<const char *>("\r\n");
    }

    /// @brief Prompt string
    ///
    void write_prompt()
    {
        write_newline();
        console->set<const char *>(">");
    }

    /// @brief Writes the current command to the console.
    ///
    void write_current_cmd()
    {
        write_newline();
        write_prompt();
        console->set<const char *>(current_cmd);
    }

    /// @brief Advances the FSM.
    ///
    void advance_state(CLIState next_state, bool advance)
    {
        current_state = next_state;

        if (advance)
        {
            event::post(event::ID::control_UpdateCLIState, nullptr);
        }
    }

    /// @brief Attempts basic tab completion. A command can be completed if:
    /// 1) It matches the command that has been entered thus far.
    /// 2) There is only one match in the existing command list.
    ///
    /// If 1 holds, but not 2, then we cannot do tab completion.
    ///
    /// @return True if the tab complete succeeded.
    ///
    bool attempt_completion()
    {
        uint32_t     size     = 0;
        const char **cmd_list = command::get_name_list(&size);
        REQUIRE(size > 0, error::TooSmall);

        uint32_t    cmd_len         = strlen(current_cmd);
        const char *remaining_chars = nullptr;
        for (uint32_t i = 0; i < size; i++)
        {
            bool previous_match = remaining_chars != nullptr;
            bool matches        = strncmp(current_cmd, cmd_list[i], cmd_len) == 0;
            if (matches && !previous_match)
            {
                remaining_chars = &cmd_list[i][strlen(current_cmd)];
            }
            else if (matches && previous_match)
            {
                remaining_chars = nullptr;
                break;
            }
        }

        if (remaining_chars == nullptr)
        {
            return false;
        }

        uint32_t remaining_len = strlen(remaining_chars);
        INVAR(cmd_len + remaining_len <= control::CMD_STR_LEN, error::InvalidLength);

        strncpy(&current_cmd[current_position], remaining_chars, remaining_len + 1);
        current_cmd[control::CMD_STR_LEN] = '\0'; // Truncate if too long
        current_position += remaining_len;

        return true;
    }

    /// @brief  Lists out out matching commands.
    ///
    /// After the list has been outputted, this function will re-write the
    /// prompt, and whatever has been entered thusfar.
    ///
    void list_matches()
    {
        uint32_t     size     = 0;
        const char **cmd_list = command::get_name_list(&size);
        REQUIRE(size > 0, error::TooSmall);

        write_newline();

        uint32_t cmd_len = strlen(current_cmd);
        for (uint32_t i = 0; i < size; i++)
        {
            if (cmd_len == 0)
            {
                break;
            }

            if (strncmp(current_cmd, cmd_list[i], cmd_len) == 0)
            {
                console->set<const char *>((char *)cmd_list[i]);
                console->set<const char *>(" ");
            }
        }

        write_newline();
        write_prompt();
        console->set<const char *>(current_cmd);
    }

    /// @brief Handles the case where backspace was sent when nothing has been entered yet. If
    /// echoing, this will delete the prompt, so we need to rewrite it.
    ///
    void handle_null_backspace()
    {
        if (ECHO_INPUT)
        {
            write_prompt();
        }
    }

    /// @brief Process the given character.
    /// @param cmd_char Character to process.
    /// @return True if the command needs to be executed.
    ///
    bool process_character(char cmd_char)
    {
        current_position
            = (current_position > control::CMD_STR_LEN) ? control::CMD_STR_LEN : current_position;

        current_cmd[current_position] = '\0';

        bool newline   = (cmd_char == '\r') || (cmd_char == '\n');
        bool tab       = cmd_char == '\t';
        bool backspace = (cmd_char == '\b') || (cmd_char == 0x7F);

        bool exec_cmd = false;

        if (newline)
        {
            current_position = 0;
            exec_cmd         = true;
        }
        else if (tab && attempt_completion())
        {
            write_current_cmd();
            current_position = current_position;
            exec_cmd         = false;
        }
        else if (tab)
        {
            list_matches();
            current_position = current_position;
            exec_cmd         = false;
        }
        else if (backspace && (current_position == 0))
        {
            handle_null_backspace();
            current_position = current_position;
            exec_cmd         = false;
        }
        else if (backspace)
        {
            current_cmd[current_position] = '\0';
            current_position--;
            exec_cmd = false;
        }
        else
        {
            current_cmd[current_position] = cmd_char;
            current_position++;
            exec_cmd = false;
        }

        return exec_cmd;
    }

    /// @brief Determines if the last three characters indicate an up arrow was pressed. According
    /// to the ANSI escape standard (https://en.wikipedia.org/wiki/ANSI_escape_code) these should be
    /// (in order) "ESC" "[" "A".
    ///
    bool is_up_arrow(char curr_char)
    {
        REQUIRE(current_position >= 2, error::InvalidPos);

        bool esc    = current_cmd[current_position - 2] == 0x1B;
        bool lbrack = current_cmd[current_position - 1] == 0x5B;
        bool a      = curr_char == 0x41;

        return esc && lbrack && a;
    }

    /// @brief Determines if the last three characters indicate a down arrow was pressed. According
    /// to the ANSI escape standard (https://en.wikipedia.org/wiki/ANSI_escape_code) these should be
    /// (in order) "ESC" "[" "B".
    ///
    bool is_down_arrow(char curr_char)
    {
        REQUIRE(current_position >= 2, error::InvalidPos);

        bool esc    = current_cmd[current_position - 2] == 0x1B;
        bool lbrack = current_cmd[current_position - 1] == 0x5B;
        bool b      = curr_char == 0x42;

        return esc && lbrack && b;
    }

    /// @brief Saves the current command in the history list (list is currently of size 1).
    ///
    void save_last_cmd()
    {
        strncpy(last_cmd, current_cmd, control::CMD_STR_LEN + 1);
        last_cmd[control::CMD_STR_LEN] = '\0'; // Truncate if too long
    }

    /// @brief Loads the last command from the history list into the current command buffer.
    ///
    void load_last_cmd()
    {
        if (current_position != 0)
        {
            write_prompt();
        }

        console->set<const char *>(last_cmd);

        strncpy(current_cmd, last_cmd, control::CMD_STR_LEN + 1);
        current_cmd[control::CMD_STR_LEN] = '\0';

        current_position = strlen(last_cmd);
    }

    /// @brief Resets the state of the current command, and reprints the prompt to indicate state
    /// reset.
    ///
    void reset_cmd()
    {
        memset(current_cmd, '\0', control::CMD_STR_LEN);

        current_position = 0;

        write_newline();
    }

    /// @brief Software echo.
    /// @param rcvd_char Character to print.
    ///
    void echo(char rcvd_char)
    {
        const uint32_t list_size = sizeof(ECHO_EXCEPTIONS) / sizeof(ECHO_EXCEPTIONS[0]);

        bool in_exception_list = false;
        for (uint32_t i = 0; i < list_size; i++)
        {
            if (ECHO_EXCEPTIONS[i] == rcvd_char)
            {
                in_exception_list = true;
                break;
            }
        }

        if (ECHO_INPUT && !in_exception_list)
        {
            char send_str[2];
            send_str[0] = rcvd_char;
            send_str[1] = '\0';
            console->set<const char *>(send_str);
        }
    }

    /// @brief On PuTTY, an arrow key seems to require a response from the host. If you don't give
    /// it something, then the next character will be eaten up as a part of this response. I suspect
    /// this is because the last character in the command sequence is a legitimate ASCII character
    /// (depending on the arrow key), so the terminal application is eating this up to suppress it
    /// on echos.
    ///
    /// However, if we echo the value from the arrow key back to the terminal, then the terminal
    /// seems to interpret these as an arrow, and will move the cursor accordingly.
    ///
    /// Thus, we need to supress the echo, but still respond with something so that the terminal
    /// doesn't eat up the next character.
    ///
    void arrow_response()
    {
        console->set<const char *>("B"); // B = Down
    }

    /// @brief Process the UART input.
    ///
    bool process_input(const char *cmd)
    {
        REQUIRE(cmd, error::InvalidPointer);

        uint32_t cmd_len = strlen(cmd);
        if (cmd_len > control::CMD_STR_LEN)
        {
            cmd_len = control::CMD_STR_LEN;
        }

        bool exec_cmd = false;
        for (uint32_t i = 0; i < cmd_len; i++)
        {
            bool up   = (current_position < 2) ? false : is_up_arrow(cmd[i]);
            bool down = (current_position < 2) ? false : is_down_arrow(cmd[i]);

            if (up)
            {
                arrow_response();
                load_last_cmd();
            }
            else if (down)
            {
                arrow_response();
                reset_cmd();
                exec_cmd = true; // Empty command
            }
            else
            {
                echo(cmd[i]);
                exec_cmd = process_character(cmd[i]);
            }

            if (exec_cmd)
            {
                break;
            }
        }

        return exec_cmd;
    }

    /// @brief Gets the arguments list for the current command.
    ///
    char **get_args(uint32_t *size)
    {
        // Worst-case scenario, someone inputs a command and
        // arguments that are all one character in length, separated
        // by 1 space. So we know the maximum size of the argument
        // list. This isn't as memory efficient as dynamic
        // allocation, but since this is just a list of pointer, I
        // think it's fine so long as the maximum command length is
        // sufficiently small.
        constexpr uint32_t arg_list_size = (control::CMD_STR_LEN - 2) / 2;
        static char       *arg_list[arg_list_size];

        *size = 0;

        // We do a -1 here in case there is a trailing space. If the
        // last character is not a space, then it's not going to
        // effect the argument list. If the last character is a
        // space, then there is nothing after it to add to the
        // argument list, so it's also not going to effect the
        // argument list.
        uint32_t current_cmd_len = strlen(current_cmd) - 1;
        for (uint32_t i = 0; i < current_cmd_len; i++)
        {
            if (current_cmd[i] == ' ')
            {
                current_cmd[i]  = '\0';
                arg_list[*size] = &current_cmd[i + 1];
                *size += 1;
            }
        }

        return arg_list;
    }

    /// @brief Executes the command.
    ///
    void execute_command()
    {
        uint32_t     size     = 0;
        const char **cmd_list = command::get_name_list(&size);
        REQUIRE(size > 0, error::InvalidLength);

        const char *command_to_exec = nullptr;
        uint32_t    command_pos     = 0;
        for (command_pos = 0; command_pos < size; command_pos++)
        {
            uint32_t cmd_len = strlen(cmd_list[command_pos]);

            bool cmd_match      = strncmp(current_cmd, cmd_list[command_pos], cmd_len) == 0;
            bool valid_len      = strlen(current_cmd) >= cmd_len;
            bool valid_arg_list = false;
            if (valid_len)
            {
                valid_arg_list = (current_cmd[cmd_len] == ' ') || (current_cmd[cmd_len] == '\0');
            }

            if (cmd_match && valid_len && valid_arg_list)
            {
                command_to_exec = cmd_list[command_pos];
                break;
            }
        }

        if ((command_to_exec == nullptr) && (strlen(current_cmd) == 0))
        {
            return;
        }

        if (command_to_exec == nullptr)
        {
            save_last_cmd();

            write_newline();
            console->set<const char *>("Invalid Command");
            write_newline();
            console->set<const char *>("Please type 'help' for a list of commands");
            write_newline();

            return;
        }

        command::CommandFunc *cmd_func_list = command::get_func_list(&size);

        save_last_cmd();

        write_newline();

        uint32_t argc = 0;
        char   **argv = get_args(&argc);
        char    *out  = cmd_func_list[command_pos](argc, argv);
        console->set<const char *>(out);
    }

    /// @brief CLI state handler.
    ///
    void handle_state(const char *rcvd_str)
    {
        switch (current_state)
        {
            case CLIState::WaitingForInput: {
                bool     exec_cmd = process_input(rcvd_str);
                CLIState next_state
                    = exec_cmd ? CLIState::ExecutingCommand : CLIState::WaitingForInput;

                advance_state(next_state, exec_cmd);
            }
            break;

            case CLIState::ExecutingCommand:
                execute_command();
                advance_state(CLIState::WritingPrompt, true);
                break;

            case CLIState::WritingPrompt:
            default:
                write_prompt();
                advance_state(CLIState::WaitingForInput, false);
                break;
        }
    }

    // End of Anonymous Namespace
}

namespace control
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Class Function Definitions
    //----------------------------------------------------------------------------------------------

    /// @brief Event handler for CLI.
    /// @param evt Event to handle.
    /// @return Handled status
    ///
    HandleStatus CLI::handle_event(event::Event evt)
    {
        HandleStatus ret_val = HandleStatus::NotHandled;

        switch (evt.id)
        {
            case event::ID::control_UARTInput:
                if (current_state == CLIState::WaitingForInput)
                {
                    handle_state(console->get<const char *>());
                }
                else // Currently executing cmd, need to wait until done
                {
                    event::post(evt.id, evt.arg);
                }
                break;

            case event::ID::control_UpdateCLIState:
                handle_state(nullptr);
                ret_val = HandleStatus::Handled;
                break;

            case event::ID::control_CLIOutput:
                console->set<const char *>((const char *)evt.arg);

            default:
                break;
        }

        return ret_val;
    }

    /// @brief Initializes the CLI.
    ///
    void CLI::init_control()
    {
        console = dynamic_cast<uart::UART *>(output::get_by_id(io::IOID::UART_CONSOLE));
        REQUIRE(console != nullptr, error::DeviceNotFound);

        fflush(stdout);
        write_newline();
        write_header();

        current_state = CLIState::WritingPrompt;
        handle_state(0); // Write first prompt
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

namespace command_test
{

    /// @brief
    uint32_t get_max_cmd_len()
    {
        return control::CMD_STR_LEN;
    }

}

// End of File