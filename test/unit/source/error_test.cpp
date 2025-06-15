/// @file error_test.cpp
/// @author Denver Hoggatt
/// @brief Unit tests for the error module.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "error.hpp"
#include "power_hal.hpp"
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


//--------------------------------------------------------------------------------------------------
//  Private Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  Tests
//--------------------------------------------------------------------------------------------------

TEST(ErrorTest, RequirePass)
{
    bool test_val = true;
    REQUIRE(test_val, error::TestFailed);
}

TEST(ErrorTest, RequireFail)
{
    bool test_val = false;
    TEST_ERROR(REQUIRE(test_val, error::TestFailed));
}

TEST(ErrorTest, InvariantPass)
{
    bool test_val = true;
    INVAR(test_val, error::TestFailed);
}

TEST(ErrorTest, InvariantFail)
{
    bool test_val = false;
    TEST_ERROR(INVAR(test_val, error::TestFailed));
}

TEST(ErrorTest, EnsurePass)
{
    bool test_val = true;
    ENSURE(test_val, error::TestFailed);
}

TEST(ErrorTest, ENSUREFail)
{
    bool test_val = false;
    TEST_ERROR(ENSURE(test_val, error::TestFailed));
}

TEST(ErrorTest, Reboot)
{
    error::set_param(settings::ID::ErrorHandleType, (uintptr_t)error::HandlerType::Reboot, false);

    volatile bool test_val     = false;
    bool          error_thrown = false;

    INVAR(test_val, error::TestFailed);

    ASSERT_FALSE(error_thrown);
}

TEST(ErrorTest, Halt)
{
    error::set_param(settings::ID::ErrorHandleType, (uintptr_t)error::HandlerType::Halt, false);
}

TEST(ErrorTest, GetParam)
{
    uint32_t val = 0;

    error::set_param(
        settings::ID::ErrorHandleType, (uintptr_t)error::HandlerType::Exception, false);
    error::get_param(settings::ID::ErrorHandleType, (uintptr_t)(&val));

    ASSERT_EQ(val, (uint32_t)error::HandlerType::Exception);

    error::get_param(settings::ID::InvalidType, (uintptr_t)(&val));
}

TEST(ErrorTest, SetParam)
{
    error::set_param(settings::ID::ErrorHandleType, 0, false);

    TEST_ERROR(INVAR(false, error::TestFailed));

    error::set_param(settings::ID::InvalidType, 0, false);
}

// End of File