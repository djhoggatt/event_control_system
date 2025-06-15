/// @file periodic_test.cpp
/// @author Denver Hoggatt
/// @brief Unit tests for the periodic module.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "periodic_test.hpp"
#include "mutex.hpp"
#include "error.hpp"
#include "timer_osal.hpp"
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

static bool callback_called;

//--------------------------------------------------------------------------------------------------
//  Fakes/Mocks
//--------------------------------------------------------------------------------------------------

DEFINE_FFF_GLOBALS;

namespace mutex
{
    FAKE_VOID_FUNC(take, ID);
    FAKE_VOID_FUNC(give, ID);
}

namespace timer_osal
{
    FAKE_VALUE_FUNC(error::Error, start, TimerID);
    FAKE_VALUE_FUNC(error::Error, stop, TimerID);
    FAKE_VALUE_FUNC(error::Error, create, TimerID, TimerCallbackFunc, uint32_t, bool);
    FAKE_VALUE_FUNC(uint32_t, curr_time_ms);
    FAKE_VALUE_FUNC(bool, is_running, TimerID);
}

//--------------------------------------------------------------------------------------------------
//  Private Functions
//--------------------------------------------------------------------------------------------------

void periodic_callback(uint32_t curr_time_ms)
{
    UNUSED(curr_time_ms);

    callback_called = true;
}

void periodic_callback_2(uint32_t curr_time_ms)
{
    UNUSED(curr_time_ms);
}

//--------------------------------------------------------------------------------------------------
//  Tests
//--------------------------------------------------------------------------------------------------

TEST(PeriodicTest, Create)
{
    ASSERT_FALSE(periodic_test::get_enabled(periodic::ID::Test));

    periodic::create(periodic::ID::Test, 10, periodic_callback);

    ASSERT_FALSE(periodic_test::get_enabled(periodic::ID::Test));
    ASSERT_EQ(periodic_test::get_period(periodic::ID::Test), 10);
}

TEST(PeriodicTest, Start)
{
    ASSERT_FALSE(periodic_test::get_enabled(periodic::ID::Test));

    periodic::create(periodic::ID::Test, 10, periodic_callback);
    periodic::start(periodic::ID::Test);

    ASSERT_TRUE(periodic_test::get_enabled(periodic::ID::Test));

    ASSERT_EQ(timer_osal::create_fake.call_count, 1);
    ASSERT_TRUE(timer_osal::start_fake.call_count);

    periodic::start(periodic::ID::Test);

    ASSERT_TRUE(periodic_test::get_enabled(periodic::ID::Test));
    ASSERT_EQ(timer_osal::create_fake.call_count, 1);
    ASSERT_TRUE(timer_osal::start_fake.call_count);

    periodic::stop(periodic::ID::Test);
}

TEST(PeriodicTest, Run)
{
    ASSERT_FALSE(periodic_test::get_enabled(periodic::ID::Test));

    callback_called = false;

    periodic::create(periodic::ID::Test, 10, periodic_callback);
    periodic::start(periodic::ID::Test);

    timer_osal::curr_time_ms_fake.return_val = 0;

    ASSERT_TRUE(timer_osal::create_fake.call_count);

    timer_osal::TimerCallbackFunc callback = timer_osal::create_fake.arg1_val;
    callback(5);
    ASSERT_FALSE(callback_called);

    callback(10);
    ASSERT_TRUE(callback_called);

    callback_called = false;
    callback(25);
    ASSERT_TRUE(callback_called);

    periodic::stop(periodic::ID::Test);
}

TEST(PeriodicTest, Stop)
{
    ASSERT_FALSE(periodic_test::get_enabled(periodic::ID::Test));

    periodic::create(periodic::ID::Test, 10, periodic_callback);
    periodic::start(periodic::ID::Test);

    ASSERT_TRUE(periodic_test::get_enabled(periodic::ID::Test));

    periodic::stop(periodic::ID::Test);
    ASSERT_TRUE(timer_osal::stop_fake.call_count);
    ASSERT_FALSE(periodic_test::get_enabled(periodic::ID::Test));

    periodic::stop(periodic::ID::Test);
    ASSERT_FALSE(periodic_test::get_enabled(periodic::ID::Test));
}

TEST(PeriodicTest, CreateMultiple)
{
    periodic::create(periodic::ID::Test, 10, periodic_callback);
    periodic::create(periodic::ID::Test, 10, periodic_callback);

    periodic::start(periodic::ID::Test);

    periodic::create(periodic::ID::Test, 10, periodic_callback);

    TEST_ERROR(periodic::create(periodic::ID::Test, 20, periodic_callback));
    TEST_ERROR(periodic::create(periodic::ID::Test, 10, periodic_callback_2));

    periodic::stop(periodic::ID::Test);
}

// End of File