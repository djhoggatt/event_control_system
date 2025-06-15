/// @file task_open_test.cpp
/// @author Denver Hoggatt
/// @brief Unit tests for the open task module.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "task_open.hpp"
#include "task_open_test.hpp"
#include "task.hpp"
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
    FAKE_VOID_FUNC(broadcast, Signal, Func);

    FAKE_VALUE_FUNC(uint32_t, num);
    FAKE_VALUE_FUNC(uint32_t, wait_any);
}

namespace setup
{
    FAKE_VOID_FUNC(open);
}

//--------------------------------------------------------------------------------------------------
//  Private Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  Tests
//--------------------------------------------------------------------------------------------------

TEST(TaskOpenTest, TaskFunc)
{
    task_open_test::terminate();

    task::num_fake.return_val      = 16;
    task::wait_any_fake.return_val = 0xFFFF;

    task_open::task_func(nullptr);

    // If we made it through, then open synchronization succeeded.
}

// End of File