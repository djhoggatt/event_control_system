/// @file control_eventPrint_test.cpp
/// @author Denver Hoggatt
/// @brief Unit tests for the control_eventPrint module.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "control.hpp"
#include "event.hpp"
#include "output.hpp"
#include "uart.hpp"
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

static uart::UART console;

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

    FAKE_VALUE_FUNC(output::Output *, get_by_id, io::IOID);
}

namespace uart
{
    FAKE_VALUE_FUNC(void *, get_by_id_override);
    void *UART::get_by_id()
    {
        return get_by_id_override();
    }

    FAKE_VOID_FUNC(set_output_override, void *);
    void UART::set_output(void *data)
    {
        set_output_override(data);
    }

    FAKE_VOID_FUNC(init_override);
    void UART::init()
    {
        init_override();
    }

    FAKE_VOID_FUNC(print_override, void *, io::IODirection);
    void UART::print(void *data, io::IODirection dir)
    {
        print_override(data, dir);
    }
}

namespace event
{
    FAKE_VALUE_FUNC(task::ID, get_associated_task, ID);
    FAKE_VALUE_FUNC(QueueInfo, get_queue_info, task::ID);
    FAKE_VALUE_FUNC(uint32_t, count, ID);
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

static control::EvtPrint *init_ctrl()
{
    static control::EvtPrint ctrl;
    static bool              inited = false;

    if (!inited)
    {
        console.uart_port   = uart::VirtualPort::UART_CLI;
        console.id          = io::IOID::UART_CONSOLE;
        console.output_type = &typeid(const char *);

        output::get_by_id_fake.return_val = (output::Output *)(&console);

        ctrl.init_control();

        inited = true;
    }

    return &ctrl;
}

//--------------------------------------------------------------------------------------------------
//  Tests
//--------------------------------------------------------------------------------------------------

TEST(ControlEvtPrintTest, Init)
{
    control::EvtPrint *ctrl = init_ctrl();

    event::Event evt;
    evt.id = event::ID::control_TestEvent;

    control::HandleStatus ret = ctrl->handle_event(evt);

    ASSERT_EQ(ret, control::HandleStatus::NotHandled);
}

TEST(ControlEvtPrintTest, SetGetParam)
{
    control::EvtPrint *ctrl = init_ctrl();

    int32_t status = ctrl->set_param(settings::ID::TestInt, 0, false);

    ASSERT_EQ(status, (int32_t)error::UnknownType);

    status = ctrl->get_param(settings::ID::TestInt, 0);

    ASSERT_EQ(status, (int32_t)error::UnknownType);
}

// End of File