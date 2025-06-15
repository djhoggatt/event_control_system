/// @file utility_test.cpp
/// @author Denver Hoggatt
/// @brief Unit tests for the utility module.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "utility.hpp"
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

TEST(UtilityTest, IsLittleEndian)
{
    ASSERT_TRUE(utility::is_little_endian());
}

TEST(UtilityTest, SwapByteOrder)
{
    uint8_t data[] = { 0x12, 0x34, 0x56, 0x78 };

    utility::swap_byte_order(data, 4, true);

    ASSERT_EQ(data[0], 0x78);
    ASSERT_EQ(data[1], 0x56);
    ASSERT_EQ(data[2], 0x34);
    ASSERT_EQ(data[3], 0x12);
}

TEST(UtilityTest, NoSwapByteOrder)
{
    uint8_t data[] = { 0x12, 0x34, 0x56, 0x78 };

    utility::swap_byte_order(data, 4, false);

    ASSERT_EQ(data[0], 0x12);
    ASSERT_EQ(data[1], 0x34);
    ASSERT_EQ(data[2], 0x56);
    ASSERT_EQ(data[3], 0x78);
}

TEST(UtilityTest, SwapByteOrderZeroLen)
{
    uint8_t data[] = { 0x12, 0x34, 0x56, 0x78 };

    utility::swap_byte_order(data, 0, true);

    ASSERT_EQ(data[0], 0x12);
    ASSERT_EQ(data[1], 0x34);
    ASSERT_EQ(data[2], 0x56);
    ASSERT_EQ(data[3], 0x78);
}

// End of File