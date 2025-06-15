/// @file version_test.cpp
/// @author Denver Hoggatt
/// @brief Unit tests for the version module.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "version.hpp"
#include "version_info.hpp"
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


//--------------------------------------------------------------------------------------------------
//  Private Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  Tests
//--------------------------------------------------------------------------------------------------

TEST(VersionTest, GetParamString)
{
    char     *version_str = nullptr;
    uintptr_t val         = (uintptr_t)&version_str;

    version::get_param(settings::ID::VersionString, val);

    ASSERT_STREQ(version_str, VERSION_STR);
}

TEST(VersionTest, SetParam)
{
    uintptr_t val     = 0;
    int32_t   ret_val = version::set_param(settings::ID::VersionString, val, false);

    ASSERT_EQ((error::Error)ret_val, error::WriteFailed);
}

// End of File