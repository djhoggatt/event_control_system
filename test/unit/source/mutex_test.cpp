/// @file mutex_test.cpp
/// @author Denver Hoggatt
/// @brief Unit tests for the mutex module.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "mutex.hpp"
#include "error.hpp"
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

namespace mutex_osal
{
    FAKE_VALUE_FUNC(error::Error, give, uint32_t);
    FAKE_VALUE_FUNC(error::Error, take, uint32_t);
}

//--------------------------------------------------------------------------------------------------
//  Private Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  Tests
//--------------------------------------------------------------------------------------------------

TEST(MutexTest, GivePreCond)
{
    TEST_ERROR(mutex::give(mutex::ID::NumIDs));
}

TEST(MutexTest, Give)
{
    uint32_t i = 0;
    for (i = 0; i < (uint32_t)mutex::ID::NumIDs; i++)
    {
        mutex::give(static_cast<mutex::ID>(i));
    }

    ASSERT_EQ(mutex_osal::give_fake.call_count, i);
}

TEST(MutexTest, GivePostCond)
{
    mutex_osal::give_fake.return_val = error::TestFailed;

    TEST_ERROR(mutex::give(mutex::ID::NumIDs));
}

TEST(MutexTest, TakePreCond)
{
    TEST_ERROR(mutex::take(mutex::ID::NumIDs));
}

TEST(MutexTest, Take)
{
    uint32_t i = 0;
    for (i = 0; i < (uint32_t)mutex::ID::NumIDs; i++)
    {
        mutex::take(static_cast<mutex::ID>(i));
    }

    ASSERT_EQ(mutex_osal::take_fake.call_count, i);
}

TEST(MutexTest, TakePostCond)
{
    mutex_osal::take_fake.return_val = error::TestFailed;

    TEST_ERROR(mutex::take(mutex::ID::NumIDs));
}

// End of File