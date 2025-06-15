/// @file command.cpp
/// @author Denver Hoggatt
/// @brief Command handling
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "command.hpp"
#include "error.hpp"
#include "macros.hpp"
#include "control.hpp"
#include "adc.hpp"
#include "mem_hal.hpp"
#include "settings.hpp"
#include "flash_hal.hpp"
#include "power_hal.hpp"

#include <cstdint>
#include <cstring>
#include <cinttypes>
#include <cstdlib>
#include <cstdio>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------

namespace
{
    //----------------------------------------------------------------------------------------------
    //  Private Constants
    //----------------------------------------------------------------------------------------------

    constexpr char NEWLINE[]      = "\r\n";
    constexpr char INVALID_ARGS[] = "Invalid Number of Arguments\r\n";

    //----------------------------------------------------------------------------------------------
    //  Private Data Types
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Private Function Prototypes
    //----------------------------------------------------------------------------------------------

#define DEF(str, func, descr) char *func(uint32_t argc, char **argv);
#include "commands.def"
#undef DEF

    //----------------------------------------------------------------------------------------------
    //  File Variables
    //----------------------------------------------------------------------------------------------

    const char *cmd_names[] = {
#define DEF(str, func, descr) str,
#include "commands.def"
#undef DEF
    };

    command::CommandFunc cmd_funcs[] = {
#define DEF(str, func, descr) func,
#include "commands.def"
#undef DEF
    };


    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------

    input::Input *_get_input_ptr(char *name_or_id)
    {
        uint32_t id = strtoul(name_or_id, NULL, 10);

        input::Input *ret_val;

        if (id == (uint32_t)io::IOID::InvalidID)
        {
            ret_val = input::get_by_name(name_or_id);
        }
        else
        {
            ret_val = input::get_by_id((io::IOID)id);
        }

        return ret_val;
    }

    char *_get_input_val(input::Input *input)
    {
        static char ret_val[command::MAX_STR_LEN];

        if (*(input->input_type) == typeid(float *))
        {
            float *ptr = input->get<float *>();
            sprintf(ret_val, "%f\r\n", (double)(*ptr));
        }
        else if (*(input->input_type) == typeid(bool))
        {
            sprintf(ret_val, "%1d\r\n", input->get<bool>());
        }
        else if (*(input->input_type) == typeid(uint32_t))
        {
            sprintf(ret_val, "%" PRIu32 "\r\n", input->get<uint32_t>());
        }
        else if (*(input->input_type) == typeid(int32_t))
        {
            sprintf(ret_val, "%" PRId32 "\r\n", input->get<int32_t>());
        }
        else if (*(input->input_type) == typeid(char *))
        {
            sprintf(ret_val, "%s\r\n", input->get<char *>());
        }
        else
        {
            sprintf(ret_val, "%s", input->cmd_input());
        }

        return ret_val;
    }

    output::Output *_get_output_ptr(char *name_or_id)
    {
        uint32_t id = strtoul(name_or_id, NULL, 10);

        output::Output *ret_val;

        if (id == (uint32_t)io::IOID::InvalidID)
        {
            ret_val = output::get_by_name(name_or_id);
        }
        else
        {
            ret_val = output::get_by_id((io::IOID)id);
        }

        return ret_val;
    }

    char *help_func(uint32_t argc, char **argv)
    {
        UNUSED(argc);
        UNUSED(argv);

        static char descriptions[] =
#define DEF(str, func, descr) str ": " descr "\r\n"
#include "commands.def"
#undef DEF
            ;

        return descriptions;
    }

    char *control_on(uint32_t argc, char **argv)
    {
        if (argc < 1)
        {
            return (char *)INVALID_ARGS;
        }

        for (uint32_t i = 0; i < argc; i++)
        {
            control::Control *ctrl = control::get_control_by_name(argv[i]);

            if (ctrl != nullptr)
            {
                ctrl->enabled = true;
            }
        }

        return (char *)NEWLINE;
    }

    char *control_off(uint32_t argc, char **argv)
    {
        if (argc < 1)
        {
            return (char *)INVALID_ARGS;
        }

        for (uint32_t i = 0; i < argc; i++)
        {
            control::Control *ctrl = control::get_control_by_name(argv[i]);

            if (ctrl != nullptr)
            {
                ctrl->enabled = false;
            }
        }

        return (char *)NEWLINE;
    }

    char *control_list(uint32_t argc, char **argv)
    {
        UNUSED(argc);
        UNUSED(argv);

        return control::get_list_of_controls();
    }

    char *get_input(uint32_t argc, char **argv)
    {
        if (argc < 2)
        {
            return (char *)INVALID_ARGS;
        }

        input::Input *input = _get_input_ptr(argv[0]);
        if (input == nullptr)
        {
            static char invalid[] = "Invalid Input\r\n";
            return invalid;
        }

        return _get_input_val(input);
    }

    char *set_output(uint32_t argc, char **argv)
    {
        if (argc < 2)
        {
            return (char *)INVALID_ARGS;
        }

        output::Output *output = _get_output_ptr(argv[0]);
        if (output == nullptr)
        {
            static char invalid[] = "Invalid Output\r\n";
            return invalid;
        }

        uint32_t value = (uintptr_t)strtoul(argv[1], NULL, 10);
        if (*(output->output_type) == typeid(bool))
        {
            output->set<bool>((bool)value);
        }
        else if (*(output->output_type) == typeid(uint32_t))
        {
            output->set<uint32_t>((uint32_t)value);
        }
        else if (*(output->output_type) == typeid(int32_t))
        {
            output->set<int32_t>((int32_t)value);
        }
        else if (*(output->output_type) == typeid(char *))
        {
            output->set<char *>((char *)value);
        }
        else
        {
            output->cmd_output(argc - 1, &argv[1]);
        }

        return (char *)NEWLINE;
    }

    char *io_print(uint32_t argc, char **argv)
    {
        uint32_t id             = 0;
        bool     valid_num_args = (argc >= 1);
        if (valid_num_args)
        {
            id = strtoul(argv[0], NULL, 10);
        }

        io::IO *io_ptr = nullptr;
        if (valid_num_args && (id == (uint32_t)io::IOID::InvalidID))
        {
            io_ptr = io::get_by_name(argv[0]); // Use name
        }
        else
        {
            io_ptr = io::get_by_id((io::IOID)id); // Use ID
        }

        if (io_ptr != nullptr)
        {
            io_ptr->print_io = true;
        }
        else
        {
            printf("Unrecognized I/O\r\n");
        }

        return (char *)NEWLINE;
    }

    char *io_quiet(uint32_t argc, char **argv)
    {
        uint32_t id             = 0;
        bool     valid_num_args = (argc >= 1);
        if (valid_num_args)
        {
            id = strtoul(argv[0], NULL, 10);
        }

        io::IO *io_ptr = nullptr;
        if (valid_num_args && (id == (uint32_t)io::IOID::InvalidID))
        {
            io_ptr = io::get_by_name(argv[0]); // Use name
        }
        else
        {
            io_ptr = io::get_by_id((io::IOID)id); // Use ID
        }

        if (io_ptr != nullptr)
        {
            io_ptr->print_io = false;
        }
        else
        {
            printf("Unrecognized I/O\r\n");
        }

        return (char *)NEWLINE;
    }

    char *io_list(uint32_t argc, char **argv)
    {
        UNUSED(argc);
        UNUSED(argv);

        constexpr uint32_t numids = (uint32_t)io::IOID::NumIDs;

        constexpr char     first_line[]   = "ID    Name\r\n";
        constexpr uint32_t first_line_len = const_str_len(first_line);

        constexpr char     spaces[]         = "     ";
        constexpr uint32_t spaces_len       = const_str_len(spaces);
        constexpr uint32_t total_spaces_len = spaces_len * numids;

        constexpr uint32_t id_len       = count_digits(numids);
        constexpr uint32_t total_id_len = id_len * numids;
        static_assert(spaces_len > id_len);

        constexpr uint32_t newline_len       = const_str_len(NEWLINE);
        constexpr uint32_t total_newline_len = newline_len * numids;

        constexpr uint32_t str_len = first_line_len + newline_len + TOTAL_IO_NAME_SIZE
                                     + total_spaces_len + total_id_len + total_newline_len;
        static char ret_val[str_len + 1]; // +1 \0
        ret_val[0] = '\0';                // Reset from previous call

        strcat(ret_val, first_line);
        strcat(ret_val, NEWLINE);

        for (uint32_t i = 0; i < numids; i++)
        {
            io::IO *i_o = (io::IO *)input::get_by_id((io::IOID)i);
            if (i_o == nullptr)
            {
                i_o = output::get_by_id((io::IOID)i);
            }

            if (i_o == nullptr)
            {
                continue; // Only show IO found in input or output lists
            }

            char id_str[id_len + 1]; //+1 \0
            snprintf(id_str, id_len + 1, "%" PRIu32 "", i);

            strcat(ret_val, id_str);
            strcat(ret_val, &spaces[count_digits(i) - 1]);
            strcat(ret_val, i_o->name);
            strcat(ret_val, NEWLINE);
        }

        ENSURE(strlen(ret_val) <= str_len, error::InvalidLength);

        return ret_val;
    }

    char *mem_list(uint32_t argc, char **argv)
    {
        bool dump = (argc > 0) && (strcmp(argv[0], "dump") == 0);

        mem_hal::HeapInfo heap_info = mem_hal::get_heap_info();
        printf("Heap Usage:\r\n");
        printf("Heap Start            (addr): %p\r\n", heap_info.base);
        printf("Heap End              (addr): %p\r\n", heap_info.end);
        printf("Heap Size            (bytes): %" PRIu32 "\r\n",
               (uint32_t)(heap_info.end - heap_info.base));
        printf("Heap Max Used        (bytes): %" PRIu32 "\r\n",
               (uint32_t)(heap_info.max - heap_info.base));
        printf("%s", NEWLINE);

        task::print_maximum_stack_usage(dump);

        uint8_t *stack_pointer = mem_hal::get_stack_pointer();
        printf("Current Stack Pointer (addr): %p\r\n", stack_pointer);

        return (char *)NEWLINE;
    }

    char *setting_set(uint32_t argc, char **argv)
    {
        char *ret_val = (char *)INVALID_ARGS;

        if (argc >= 2)
        {
            uint32_t setting_id = strtoul(argv[0], NULL, 10);

            settings::set((settings::ID)setting_id, (const char *)argv[1], true);

            ret_val = (char *)NEWLINE;
        }

        return ret_val;
    }

    char *setting_get(uint32_t argc, char **argv)
    {
        char *ret_val = (char *)INVALID_ARGS;

        if (argc >= 1)
        {
            uint32_t setting_id = strtoul(argv[0], NULL, 10);

            static char setting_val[settings::MAX_STR_LEN + 3];

            settings::get((settings::ID)setting_id, setting_val);

            setting_val[strlen(setting_val)]     = '\r';
            setting_val[strlen(setting_val) + 1] = '\n';
            setting_val[strlen(setting_val) + 2] = '\0';

            ret_val = setting_val;
        }

        return ret_val;
    }

    char *flash_write(uint32_t argc, char **argv)
    {
        char *ret_val = (char *)INVALID_ARGS;

        if (argc > 1)
        {
            uint32_t data = strtoul(argv[0], NULL, 16);
            uint32_t addr = strtoul(argv[1], NULL, 16);
            flash_hal::write(addr, (uint8_t *)&data, sizeof(uint32_t));

            ret_val = (char *)NEWLINE;
        }

        return ret_val;
    }

    char *flash_read(uint32_t argc, char **argv)
    {
        char *ret_val = (char *)INVALID_ARGS;

        if (argc <= 0)
        {
            return ret_val;
        }

        uint32_t addr = strtoul(argv[0], NULL, 16);
        uint32_t data = 0;
        flash_hal::read(addr, (uint8_t *)&data, sizeof(uint32_t));

        constexpr uint32_t str_len = 14; // 0x + 8 + ' ' + \r\n + \0
        static char        data_str[str_len];
        snprintf(data_str, str_len, "0x%08" PRIX32 " \r\n", data);

        ret_val = data_str;

        return ret_val;
    }

    char *flash_erase(uint32_t argc, char **argv)
    {
        char *ret_val = (char *)INVALID_ARGS;

        if (argc <= 0)
        {
            return ret_val;
        }

        uint32_t addr = strtoul(argv[0], NULL, 16);
        flash_hal::erase(addr);

        ret_val = (char *)NEWLINE;

        return ret_val;
    }

    char *reboot(uint32_t argc, char **argv)
    {
        UNUSED(argc);
        UNUSED(argv);

        printf("\r\n");

        power_hal::reset();

        return (char *)NEWLINE;
    }

    // End of Anonymous Namespace
}

namespace command
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    /// @brief Returns the list of command functions.
    /// @param size Will be set to the size of the list.
    /// @return Command functions.
    ///
    CommandFunc *get_func_list(uint32_t *size)
    {
        REQUIRE(size != nullptr, error::InvalidPointer);

        *size = sizeof(cmd_funcs) / sizeof(cmd_funcs[0]);

        uint32_t name_lst_size = sizeof(cmd_names) / sizeof(cmd_names[0]);

        ENSURE(*size == name_lst_size, error::InvalidLength);

        return cmd_funcs;
    }

    /// @brief Returns the list of command names.
    /// @param size Will be set to the size of the list.
    /// @return Command names.
    ///
    const char **get_name_list(uint32_t *size)
    {
        REQUIRE(size != nullptr, error::InvalidPointer);

        *size = sizeof(cmd_names) / sizeof(cmd_names[0]);

        uint32_t func_lst_size = sizeof(cmd_funcs) / sizeof(cmd_funcs[0]);

        ENSURE(*size == func_lst_size, error::InvalidLength);

        return cmd_names;
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

namespace command_test
{


}