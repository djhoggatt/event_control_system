/// @file control_test.cpp
/// @author Denver Hoggatt
/// @brief Unit tests for the control module.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "control_test.hpp"
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

event::Event          rcvd_event_1;
event::Event          rcvd_event_2;
control::HandleStatus ret_status;

//--------------------------------------------------------------------------------------------------
//  Fakes/Mocks
//--------------------------------------------------------------------------------------------------

DEFINE_FFF_GLOBALS;

namespace control
{

#undef TESTING // Define non-testing controls for compilation
#define DEF(CONTROL_NAME, CONTROL_STR, ENABLED)               \
    HandleStatus CONTROL_NAME::handle_event(event::Event evt) \
    {                                                         \
        UNUSED(evt);                                          \
        return HandleStatus::NotHandled;                      \
    }                                                         \
                                                              \
    void CONTROL_NAME::init_control()                         \
    {                                                         \
    }
#include "controls.def"
#undef DEF
#define TESTING

    HandleStatus TestControl1::handle_event(event::Event evt)
    {
        rcvd_event_1 = evt;

        return ret_status;
    }

    HandleStatus TestControl2::handle_event(event::Event evt)
    {
        rcvd_event_2 = evt;

        return ret_status;
    }

    void TestControl1::init_control()
    {
    }

    void TestControl2::init_control()
    {
    }
}

//--------------------------------------------------------------------------------------------------
//  Private Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  Tests
//--------------------------------------------------------------------------------------------------

TEST(ControlTest, Init)
{
    control::open();

    control::Control **controls = control_test::get_controls();
    for (uint32_t i = 0; i < (uint32_t)control::ID::NumIDs; i++)
    {
        ASSERT_NE(controls[i], nullptr);
    }
}

TEST(ControlTest, DisperseEvent)
{
    control::open();

    event::Event evt;

    rcvd_event_1.id = event::ID::NullEvent;
    evt.id          = event::ID::control_TestEvent;
    ret_status      = control::HandleStatus::NotHandled;

    control_test::get_controls()[0]->enabled = false;
    control::disperse_event(evt);
    ASSERT_EQ(rcvd_event_1.id, event::ID::NullEvent);

    control_test::get_controls()[0]->enabled = true;
    control::disperse_event(evt);
    ASSERT_EQ(rcvd_event_1.id, event::ID::control_TestEvent);
}

TEST(ControlTest, HandleStatus)
{
    control::open();

    control::TestControl2 ctrl_override;
    event::Event          evt;

    rcvd_event_1.id       = event::ID::NullEvent;
    rcvd_event_2.id       = event::ID::NullEvent;
    evt.id                = event::ID::control_TestEvent;
    ctrl_override.enabled = true;

    control_test::override_control(1, &ctrl_override);

    ret_status = control::HandleStatus::Handled;
    control::disperse_event(evt);
    ASSERT_EQ(rcvd_event_1.id, event::ID::control_TestEvent);
    ASSERT_EQ(rcvd_event_2.id, event::ID::NullEvent);

    rcvd_event_1.id = event::ID::NullEvent;
    rcvd_event_2.id = event::ID::NullEvent;

    ret_status = control::HandleStatus::NotHandled;
    control::disperse_event(evt);
    ASSERT_EQ(rcvd_event_1.id, event::ID::control_TestEvent);
    ASSERT_EQ(rcvd_event_2.id, event::ID::control_TestEvent);
}

TEST(ControlTest, GetControlPreCond)
{
    TEST_ERROR(control::get_control_by_name(nullptr));
}

TEST(ControlTest, GetControl)
{
    control::open();

    control::Control *test_control = control_test::get_controls()[0];

    control::Control *ctrl = control::get_control_by_name(test_control->name);

    ASSERT_EQ(test_control, ctrl);
}

TEST(ControlTest, GetList)
{
    control::open();

    char *list = control::get_list_of_controls();

    ASSERT_NE(list, nullptr);
}

TEST(ControlTest, GetSetParam)
{
    control::get_param(settings::ID::TestInt, 0);

    control::set_param(settings::ID::TestInt, 0, false);
}

// End of File