/// @file event_test.cpp
/// @author Denver Hoggatt
/// @brief Unit tests for the event module.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "event.hpp"
#include "task.hpp"
#include "error.hpp"
#include "mutex.hpp"
#include "macros.hpp"
#include "fff.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

//--------------------------------------------------------------------------------------------------
//  Private Constants
//--------------------------------------------------------------------------------------------------

constexpr uint32_t STRESS_MULTIPLIER = 5;
constexpr uint32_t TEST_VAL          = 500;
constexpr uint32_t MAX_SLEEP_TIME    = 10;

//--------------------------------------------------------------------------------------------------
//  File Variables
//--------------------------------------------------------------------------------------------------

bool starved = false;
bool done    = false;

//--------------------------------------------------------------------------------------------------
//  Fakes/Mocks
//--------------------------------------------------------------------------------------------------

DEFINE_FFF_GLOBALS;

namespace task
{
    FAKE_VOID_FUNC(send_signal, ID, Signal);
}

namespace mutex
{
    FAKE_VOID_FUNC(take, ID);
    FAKE_VOID_FUNC(give, ID);
}

//--------------------------------------------------------------------------------------------------
//  Private Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  Tests
//--------------------------------------------------------------------------------------------------

TEST(EventTest, Init)
{
    event::init();
    event::init();
}

TEST(EventTest, PostEvent)
{
    event::init();

    event::post(event::ID::control_TestEvent, nullptr);

    ASSERT_NE(task::send_signal_fake.call_count, 0);
}

TEST(EventTest, HandleEventNull)
{
    event::init();

    event::Event evt = event::handle(task::ID::control);
    ASSERT_EQ(evt.id, event::ID::NullEvent);
}

TEST(EventTest, HandleEvent)
{
    event::init();

    event::post(event::ID::control_TestEvent, nullptr);
    event::Event evt = event::handle(task::ID::control);

    ASSERT_EQ(evt.id, event::ID::control_TestEvent);
    ASSERT_EQ(evt.task, task::ID::control);
    ASSERT_EQ(evt.arg, nullptr);

    evt = event::handle(task::ID::control);
    ASSERT_EQ(evt.id, event::ID::NullEvent);
}

TEST(EventTest, ArgumentPtr)
{
    event::init();

    uint32_t test_ptr = TEST_VAL;

    event::post(event::ID::control_TestEvent, &test_ptr);
    event::Event evt = event::handle(task::ID::control);

    uint32_t *rcvd_arg = (uint32_t *)evt.arg;

    ASSERT_EQ(rcvd_arg, &test_ptr);
    ASSERT_EQ(*rcvd_arg, test_ptr);
}

TEST(EventTest, Overflow)
{
    event::init();

    // - 1 because comparison is against the next position of rear, not the
    // current position. I think this is fine, since if you're at size - 1,
    // chances are you're going to overflow.
    uint32_t size = event::QUEUE_SIZE - 1;
    for (uint32_t i = 0; i < size; i++)
    {
        event::post(event::ID::control_TestEvent, nullptr);
    }

    TEST_ERROR(event::post(event::ID::control_TestEvent, nullptr));
}

TEST(EventTest, AssociatedTasks)
{
    event::init();

    event::Event evt;

    // For all events, make sure that posting is properly directed to the
    // associated task.
#define DEF(task_name, event_name)                             \
    event::post(event::ID::task_name##_##event_name, nullptr); \
    evt = event::handle(task::ID::task_name);                  \
    ASSERT_EQ(evt.id, event::ID::task_name##_##event_name);

#include "events.def"

#undef DEF
}

TEST(EventTest, GetAssociatedTask)
{
    event::init();

    task::ID id = event::get_associated_task(event::ID::control_TestEvent);
    ASSERT_EQ(id, task::ID::control);
}

TEST(EventTest, GetQueueInfo)
{
    event::init();

    event::post(event::ID::control_TestEvent, nullptr);

    task::ID         id   = event::get_associated_task(event::ID::control_TestEvent);
    event::QueueInfo info = event::get_queue_info(id);

    ASSERT_EQ(info.front_pos, 0);
    ASSERT_EQ(info.rear_pos, 1);
}

// End of File