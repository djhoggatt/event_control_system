/// @file settings_test.cpp
/// @author Denver Hoggatt
/// @brief Unit tests for the settings module.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "settings.hpp"
#include "error.hpp"
#include "version.hpp"
#include "control.hpp"
#include "macros.hpp"
#include "fff.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cstdlib>

//--------------------------------------------------------------------------------------------------
//  Private Constants
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  File Variables
//--------------------------------------------------------------------------------------------------

static char     test_str[settings::MAX_STR_LEN];
static uint32_t test_int_val;
static uint32_t test_uint_val;
static float    test_float_val;

//--------------------------------------------------------------------------------------------------
//  Fakes/Mocks
//--------------------------------------------------------------------------------------------------

DEFINE_FFF_GLOBALS;

namespace settings
{
    FAKE_VALUE_FUNC(error::Error, save_setting, ID, char *);
    FAKE_VALUE_FUNC(error::Error, load_settings);
    FAKE_VOID_FUNC(backend_init);
}

namespace settings_test
{
    FAKE_VALUE_FUNC(int32_t, get_param, settings::ID, uintptr_t);
    FAKE_VALUE_FUNC(int32_t, set_param, settings::ID, uintptr_t, bool);
}

namespace control
{
    FAKE_VALUE_FUNC(int32_t, get_param, settings::ID, uintptr_t);
    FAKE_VALUE_FUNC(int32_t, set_param, settings::ID, uintptr_t, bool);
}

//--------------------------------------------------------------------------------------------------
//  Private Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  Tests
//--------------------------------------------------------------------------------------------------

static int32_t get_param_int(settings::ID id, uintptr_t val)
{
    *((int32_t *)val) = test_int_val;

    return error::NoError;
}

static int32_t get_param_uint(settings::ID id, uintptr_t val)
{
    *((uint32_t *)val) = test_uint_val;

    return error::NoError;
}

static int32_t get_param_str(settings::ID id, uintptr_t val)
{
    *((char **)val) = test_str;

    return error::NoError;
}

static int32_t get_param_float(settings::ID id, uintptr_t val)
{
    *((float *)(void *)val) = test_float_val;

    return error::NoError;
}

//------------------------------------------------------------------------------
//  Tests
//------------------------------------------------------------------------------

TEST(SettingsTest, Init)
{
    settings::init();

    ASSERT_TRUE(settings_test::set_param_fake.call_count);

    ASSERT_EQ(settings_test::set_param_fake.arg0_history[0], settings::ID::TestInt);
    ASSERT_EQ(settings_test::set_param_fake.arg1_history[0], -1);
    ASSERT_TRUE(settings_test::set_param_fake.arg2_history[0]);

    ASSERT_EQ(settings_test::set_param_fake.arg0_history[3], settings::ID::TestStr);
    ASSERT_STREQ((char *)settings_test::set_param_fake.arg1_history[3], "Test");
    ASSERT_TRUE(settings_test::set_param_fake.arg2_history[3]);

    RESET_FAKE(settings_test::set_param);
}

TEST(SettingsTest, SetPreCond)
{
    TEST_ERROR(settings::set(settings::ID::NumSettings, "-5", true));
    TEST_ERROR(settings::set(settings::ID::TestInt, nullptr, true));
}

TEST(SettingsTest, SetInt)
{
    settings::set(settings::ID::TestInt, "-5", true);

    ASSERT_EQ(settings_test::set_param_fake.arg0_val, settings::ID::TestInt);
    ASSERT_EQ(settings_test::set_param_fake.arg1_val, -5);
    ASSERT_FALSE(settings_test::set_param_fake.arg2_val);

    RESET_FAKE(settings_test::set_param);
}

TEST(SettingsTest, SetUInt)
{
    settings::set(settings::ID::TestUInt, "7", true);

    ASSERT_EQ(settings_test::set_param_fake.arg0_val, settings::ID::TestUInt);
    ASSERT_EQ(settings_test::set_param_fake.arg1_val, 7);
    ASSERT_FALSE(settings_test::set_param_fake.arg2_val);

    RESET_FAKE(settings_test::set_param);
}

TEST(SettingsTest, SetHex)
{
    settings::set(settings::ID::TestHex, "0xA", true);

    ASSERT_EQ(settings_test::set_param_fake.arg0_val, settings::ID::TestHex);
    ASSERT_EQ(settings_test::set_param_fake.arg1_val, 0xA);
    ASSERT_FALSE(settings_test::set_param_fake.arg2_val);

    RESET_FAKE(settings_test::set_param);
}

TEST(SettingsTest, SetStr)
{
    settings::set(settings::ID::TestStr, "TestStr", true);

    ASSERT_EQ(settings_test::set_param_fake.arg0_val, settings::ID::TestStr);
    ASSERT_STREQ((char *)settings_test::set_param_fake.arg1_val, "TestStr");
    ASSERT_FALSE(settings_test::set_param_fake.arg2_val);

    RESET_FAKE(settings_test::set_param);
}

TEST(SettingsTest, SetFloat)
{
    settings::set(settings::ID::TestFloat, "1.2", true);

    ASSERT_EQ(settings_test::set_param_fake.arg0_val, settings::ID::TestFloat);
    float actual_val = *(float *)settings_test::set_param_fake.arg1_val;
    ASSERT_FLOAT_EQ(actual_val, 1.2f);
    ASSERT_FALSE(settings_test::set_param_fake.arg2_val);

    RESET_FAKE(settings_test::set_param);
}

TEST(SettingsTest, SetPermissions)
{
    // SET PERMISSION
    int32_t ret_val = settings::set(settings::ID::TestPermSet, "TestPermSet", true);

    ASSERT_NE((error::Error)ret_val, error::WriteFailed);

    RESET_FAKE(settings_test::set_param);

    // GET PERMISSION
    ret_val = settings::set(settings::ID::TestPermGet, "TestPermGet", true);

    ASSERT_EQ((error::Error)ret_val, error::WriteFailed);

    RESET_FAKE(settings_test::set_param);

    // SET AND GET PERMISSION
    ret_val = settings::set(settings::ID::TestPermSetGet, "TestPermSetGet", true);

    ASSERT_NE((error::Error)ret_val, error::WriteFailed);

    RESET_FAKE(settings_test::set_param);
}

TEST(SettingsTest, SetPostCond)
{
    TEST_ERROR(settings::set(settings::ID::InvalidType, "0", true));
}

TEST(SettingsTest, GetPreCond)
{
    char str[settings::MAX_STR_LEN];
    memset(str, 0, settings::MAX_STR_LEN);

    TEST_ERROR(settings::get(settings::ID::NumSettings, str));
    TEST_ERROR(settings::get(settings::ID::TestInt, nullptr));
}

TEST(SettingsTest, GetInt)
{
    char str[settings::MAX_STR_LEN];
    memset(str, 0, settings::MAX_STR_LEN);
    test_int_val = -5;

    settings_test::get_param_fake.custom_fake = get_param_int;
    settings::get(settings::ID::TestInt, str);

    int32_t get_int = strtol(str, NULL, 10);

    ASSERT_EQ(settings_test::get_param_fake.arg0_val, settings::ID::TestInt);
    ASSERT_EQ(test_int_val, get_int);

    RESET_FAKE(settings_test::get_param);
}

TEST(SettingsTest, GetUInt)
{
    char str[settings::MAX_STR_LEN];
    memset(str, 0, settings::MAX_STR_LEN);
    test_uint_val = 7;

    settings_test::get_param_fake.custom_fake = get_param_uint;
    settings::get(settings::ID::TestUInt, str);

    int32_t get_uint = strtoul(str, NULL, 10);

    ASSERT_EQ(settings_test::get_param_fake.arg0_val, settings::ID::TestUInt);
    ASSERT_EQ(test_uint_val, get_uint);

    RESET_FAKE(settings_test::get_param);
}

TEST(SettingsTest, GetHex)
{
    char str[settings::MAX_STR_LEN];
    memset(str, 0, settings::MAX_STR_LEN);
    test_uint_val = 9;

    settings_test::get_param_fake.custom_fake = get_param_uint;
    settings::get(settings::ID::TestHex, str);

    int32_t get_hex = strtoul(str, NULL, 16);

    ASSERT_EQ(settings_test::get_param_fake.arg0_val, settings::ID::TestHex);
    ASSERT_EQ(test_uint_val, get_hex);

    RESET_FAKE(settings_test::get_param);
}

TEST(SettingsTest, GetStr)
{
    char str[settings::MAX_STR_LEN];
    memset(str, 0, settings::MAX_STR_LEN);
    strncpy(test_str, "Testing", settings::MAX_STR_LEN);

    settings_test::get_param_fake.custom_fake = get_param_str;
    settings::get(settings::ID::TestStr, str);

    ASSERT_EQ(settings_test::get_param_fake.arg0_val, settings::ID::TestStr);
    ASSERT_STREQ(test_str, str);

    RESET_FAKE(settings_test::get_param);
}

TEST(SettingsTest, GetFloat)
{
    char str[settings::MAX_STR_LEN];
    memset(str, 0, settings::MAX_STR_LEN);
    test_float_val = 2.3f;

    settings_test::get_param_fake.custom_fake = get_param_float;
    settings::get(settings::ID::TestFloat, str);

    float parsed_float = strtof(str, nullptr);

    ASSERT_EQ(settings_test::get_param_fake.arg0_val, settings::ID::TestFloat);
    ASSERT_FLOAT_EQ(parsed_float, test_float_val);

    RESET_FAKE(settings_test::get_param);
}

TEST(SettingsTest, GetPermissions)
{
    char str[settings::MAX_STR_LEN];
    memset(str, 0, settings::MAX_STR_LEN);

    // SET PERMISSION
    int32_t ret_val = settings::get(settings::ID::TestPermSet, str);

    ASSERT_EQ((error::Error)ret_val, error::ReadFailed);

    RESET_FAKE(settings_test::get_param);

    // GET PERMISSION
    ret_val = settings::get(settings::ID::TestPermGet, str);

    ASSERT_NE((error::Error)ret_val, error::ReadFailed);

    RESET_FAKE(settings_test::get_param);

    // SET AND GET PERMISSION
    ret_val = settings::get(settings::ID::TestPermSetGet, str);

    ASSERT_NE((error::Error)ret_val, error::ReadFailed);

    RESET_FAKE(settings_test::get_param);
}

TEST(SettingsTest, GetPostCond)
{
    char str[settings::MAX_STR_LEN];
    memset(str, 0, settings::MAX_STR_LEN);

    TEST_ERROR(settings::get(settings::ID::InvalidType, str));
}

// End of File