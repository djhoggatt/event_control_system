/// @file task_control_test.cpp
/// @author Denver Hoggatt
/// @brief Unit tests for the control task module.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "task_control.hpp"
#include "task.hpp"
#include "event.hpp"
#include "input.hpp"
#include "io.hpp"
#include "control.hpp"
#include "adc_hal.hpp"
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

namespace task
{
    FAKE_VOID_FUNC(wait_strict, Signal);
    FAKE_VOID_FUNC(send_open_signal, Func);

    FAKE_VALUE_FUNC(ID, get_id, Func);
    FAKE_VALUE_FUNC(uint32_t, wait_any);
}

namespace event
{
    FAKE_VALUE_FUNC(Event, handle, task::ID);
}

namespace input
{
    FAKE_VALUE_FUNC(input::Input *, get_by_id, io::IOID);
}

namespace control
{
    FAKE_VOID_FUNC(disperse_event, event::Event);
}

namespace adc_hal
{
    FAKE_VOID_FUNC(start_conversions);
    FAKE_VOID_FUNC(restart_conversion, uint32_t);
}

//--------------------------------------------------------------------------------------------------
//  Private Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  Tests
//--------------------------------------------------------------------------------------------------

TEST(TaskControlTest, TaskFunc)
{
    task::wait_any_fake.return_val = (uint32_t)task::Signal::GlobalTerminate;

    task_control::task_func(nullptr);

    ASSERT_TRUE(task::wait_strict_fake.call_count);
    ASSERT_TRUE(task::send_open_signal_fake.call_count);

    ASSERT_TRUE(task::wait_any_fake.call_count);
}

TEST(TaskControlTest, Event)
{
    event::Event evts[2];
    evts[1].id = event::ID::NullEvent;

    uint32_t start_val = (uint32_t)event::ID::NullEvent + 1;
    for (uint32_t i = start_val; i < (uint32_t)event::ID::NumEvents; i++)
    {
        evts[0].id   = (event::ID)i;
        evts[0].task = task::ID::control;
        evts[0].arg  = nullptr;
        SET_RETURN_SEQ(event::handle, evts, sizeof(evts) / sizeof(evts[0]));

        uint32_t signals[2]
            = { (uint32_t)task::Signal::GlobalEvent, (uint32_t)task::Signal::GlobalTerminate };
        SET_RETURN_SEQ(task::wait_any, signals, 2);

        task::get_id_fake.return_val = task::ID::control;

        switch ((event::ID)i)
        {
            case event::ID::control_ADCInput:
                task_control::task_func(nullptr);
                break;

            default:
                break;
        }
    }
}

// End of File