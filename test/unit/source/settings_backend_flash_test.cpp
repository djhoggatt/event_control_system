/// @file settings_back_flash_test.cpp
/// @author Denver Hoggatt
/// @brief Unit tests for the settings_back_flash module.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "settings.hpp"
#include "settings_backend.hpp"
#include "settings_test.hpp"
#include "error.hpp"
#include "macros.hpp"
#include "fff.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cstdlib>

//--------------------------------------------------------------------------------------------------
//  Private Constants
//--------------------------------------------------------------------------------------------------

constexpr uint32_t SECTOR_SIZE = 8192;

//--------------------------------------------------------------------------------------------------
//  File Variables
//--------------------------------------------------------------------------------------------------

uint8_t func_count       = 0;
uint8_t func_count_0     = 0;
uint8_t func_count_1     = 0;
uint8_t func_count_2     = 0;
uint8_t func_count_0_ns  = 0;
uint8_t func_count_1_ns  = 0;
uint8_t func_count_2_ns  = 0;
uint8_t func_count_erase = 0;

//--------------------------------------------------------------------------------------------------
//  Fakes/Mocks
//--------------------------------------------------------------------------------------------------

DEFINE_FFF_GLOBALS;

namespace settings
{
    FAKE_VALUE_FUNC(int32_t, set, ID, const char *, bool);
}

namespace flash_hal
{
    FAKE_VALUE_FUNC(uint32_t, get_sector_size);
    FAKE_VALUE_FUNC(error::Error, read, uint32_t, uint8_t *, uint32_t);
    FAKE_VALUE_FUNC(error::Error, write, uint32_t, uint8_t *, uint32_t);
    FAKE_VALUE_FUNC(error::Error, erase, uint32_t);
}

namespace flash_hal
{
    uint32_t align(uint32_t val)
    {
        return (val + 0x7) & ~0x7;
    }
}

//--------------------------------------------------------------------------------------------------
//  Private Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  Tests
//--------------------------------------------------------------------------------------------------

namespace
{
    error::Error empty(uint32_t addr, uint8_t *buf, uint32_t size)
    {
        UNUSED(size);

        uint32_t *header = (uint32_t *)buf;
        header[0]        = 0xFFFFFFFF;
        header[1]        = 0xFFFFFFFF;
        header[2]        = 0xFFFFFFFF;
        header[3]        = 0xFFFFFFFF;

        return error::NoError;
    }

    error::Error abandoned_sector(uint32_t addr, uint8_t *buf, uint32_t size)
    {
        UNUSED(size);

        if (addr < SECTOR_SIZE)
        {
            uint32_t *header = (uint32_t *)buf;
            header[0]        = 0;
            header[1]        = 1;
            header[2]        = 0;
            header[3]        = 0xFFFFFFFF;
        }
        else
        {
            uint32_t *header = (uint32_t *)buf;
            header[0]        = 0xFFFFFFFF;
            header[1]        = 0xFFFFFFFF;
            header[2]        = 0xFFFFFFFF;
            header[3]        = 0xFFFFFFFF;
        }

        return error::NoError;
    }

    error::Error mostly_abandoned(uint32_t addr, uint8_t *buf, uint32_t size)
    {
        UNUSED(size);

        if (addr == 0)
        {
            uint32_t *header = (uint32_t *)buf;
            header[0]        = 0;
            header[1]        = 1;
            header[2]        = 0xFFFFFFFF;
            header[3]        = 0xFFFFFFFF;
        }
        else if (addr < SECTOR_SIZE)
        {
            uint32_t *header = (uint32_t *)buf;
            header[0]        = 1;
            header[1]        = 1;
            header[2]        = 0;
            header[3]        = 0xFFFFFFFF;
        }
        else
        {
            uint32_t *header = (uint32_t *)buf;
            header[0]        = 0xFFFFFFFF;
            header[1]        = 0xFFFFFFFF;
            header[2]        = 0xFFFFFFFF;
            header[3]        = 0xFFFFFFFF;
        }

        return error::NoError;
    }

    error::Error end_abandoned(uint32_t addr, uint8_t *buf, uint32_t size)
    {
        UNUSED(size);

        if (addr >= SECTOR_SIZE)
        {
            uint32_t *header = (uint32_t *)buf;
            header[0]        = 0;
            header[1]        = 1;
            header[2]        = 0;
            header[3]        = 0xFFFFFFFF;
        }
        else
        {
            uint32_t *header = (uint32_t *)buf;
            header[0]        = 0xFFFFFFFF;
            header[1]        = 0xFFFFFFFF;
            header[2]        = 0xFFFFFFFF;
            header[3]        = 0xFFFFFFFF;
        }

        return error::NoError;
    }

    error::Error two_settings(uint32_t addr, uint8_t *buf, uint32_t size)
    {
        UNUSED(size);

        if (addr == 0x00)
        {
            uint32_t *header = (uint32_t *)buf;
            header[0]        = 0;
            header[1]        = 1;
            header[2]        = 0xFFFFFFFF;
            header[3]        = 0xFFFFFFFF;
        }
        else if (addr == 0x10)
        {
            buf[0] = '1';
        }
        else if (addr == 0x18)
        {
            uint32_t *header = (uint32_t *)buf;
            header[0]        = 1;
            header[1]        = 1;
            header[2]        = 0xFFFFFFFF;
            header[3]        = 0xFFFFFFFF;
        }
        else if (addr == 0x28)
        {
            buf[0] = '2';
        }
        else
        {
            uint32_t *header = (uint32_t *)buf;
            header[0]        = 0xFFFFFFFF;
            header[1]        = 0xFFFFFFFF;
            header[2]        = 0xFFFFFFFF;
            header[3]        = 0xFFFFFFFF;
        }

        return error::NoError;
    }

    error::Error write_error_all(uint32_t addr, uint8_t *buf, uint32_t size)
    {
        UNUSED(addr);
        UNUSED(buf);
        UNUSED(size);

        return error::WriteFailed;
    }

    error::Error write_error_0(uint32_t addr, uint8_t *buf, uint32_t size)
    {
        if (func_count_0 == 0)
        {
            func_count_0++;
            return error::WriteFailed;
        }
        else
        {
            func_count_0++;
            return error::NoError;
        }
    }

    error::Error write_error_1(uint32_t addr, uint8_t *buf, uint32_t size)
    {
        if (func_count_1 == 1)
        {
            func_count_1++;
            return error::WriteFailed;
        }
        else
        {
            func_count_1++;
            return error::NoError;
        }
    }

    error::Error write_error_2(uint32_t addr, uint8_t *buf, uint32_t size)
    {
        if (func_count_2 == 2)
        {
            func_count_2++;
            return error::WriteFailed;
        }
        else
        {
            func_count_2++;
            return error::NoError;
        }
    }

    error::Error write_error_ns(uint32_t addr, uint8_t *buf, uint32_t size)
    {
        return error::WriteFailed;
    }

    error::Error write_error_0_ns(uint32_t addr, uint8_t *buf, uint32_t size)
    {
        if (func_count_0_ns == 0)
        {
            func_count_0_ns++;
            return error::WriteFailed;
        }
        else
        {
            func_count_0_ns++;
            return error::NoError;
        }
    }

    error::Error write_error_1_ns(uint32_t addr, uint8_t *buf, uint32_t size)
    {
        if (func_count_1_ns == 1)
        {
            func_count_1_ns++;
            return error::WriteFailed;
        }
        else
        {
            func_count_1_ns++;
            return error::NoError;
        }
    }

    error::Error write_error_2_ns(uint32_t addr, uint8_t *buf, uint32_t size)
    {
        if (func_count_2_ns == 2)
        {
            func_count_2_ns++;
            return error::WriteFailed;
        }
        else
        {
            func_count_2_ns++;
            return error::NoError;
        }
    }

    error::Error erase_error_all_sectors(uint32_t addr)
    {
        if (func_count_erase <= 2)
        {
            func_count_erase++;
            return error::EraseFailed;
        }
        else
        {
            return error::NoError;
        }
    }

    error::Error erase_error_loop(uint32_t addr)
    {
        return error::EraseFailed;
    }
}

//------------------------------------------------------------------------------
//  Tests
//------------------------------------------------------------------------------

TEST(SettingsBackendTest, Init)
{
    flash_hal::read_fake.custom_fake           = abandoned_sector;
    flash_hal::get_sector_size_fake.return_val = SECTOR_SIZE;

    settings::backend_init();

    ASSERT_EQ(settings_test::get_curr_sector(), SECTOR_SIZE);

    flash_hal::read_fake.custom_fake = empty;
    settings::backend_init();

    ASSERT_EQ(settings_test::get_curr_sector(), 0);

    RESET_FAKE(flash_hal::read);
    RESET_FAKE(flash_hal::get_sector_size);
}

TEST(SettingsBackendTest, Load)
{
    flash_hal::read_fake.custom_fake           = two_settings;
    flash_hal::get_sector_size_fake.return_val = SECTOR_SIZE;

    settings::load_settings();

    ASSERT_EQ(settings::set_fake.call_count, 2);

    ASSERT_EQ(settings::set_fake.arg0_history[0], (settings::ID)0);
    ASSERT_FALSE(settings::set_fake.arg2_history[0]);

    ASSERT_EQ(settings::set_fake.arg0_history[1], (settings::ID)1);
    ASSERT_FALSE(settings::set_fake.arg2_history[1]);

    RESET_FAKE(flash_hal::read);
    RESET_FAKE(flash_hal::get_sector_size);
}

TEST(SettingsBackendTest, Save)
{
    flash_hal::read_fake.custom_fake           = two_settings;
    flash_hal::get_sector_size_fake.return_val = SECTOR_SIZE;

    char str[] = "1";
    settings::save_setting((settings::ID)1, str);

    ASSERT_EQ(flash_hal::write_fake.call_count, 3); // deletion, header, data

    RESET_FAKE(flash_hal::read);
    RESET_FAKE(flash_hal::get_sector_size);
}

TEST(SettingsBackendTest, Switchover)
{
    flash_hal::read_fake.custom_fake           = mostly_abandoned;
    flash_hal::get_sector_size_fake.return_val = SECTOR_SIZE;

    char str[] = "1";
    settings::save_setting((settings::ID)0, str);

    ASSERT_GT(flash_hal::write_fake.call_count, 3);
    ASSERT_EQ(settings_test::get_curr_sector(), SECTOR_SIZE);

    // Switch back to beginning
    flash_hal::read_fake.custom_fake = end_abandoned;

    settings::save_setting((settings::ID)0, str);

    ASSERT_GT(flash_hal::write_fake.call_count, 3);
    ASSERT_EQ(settings_test::get_curr_sector(), 0);

    RESET_FAKE(flash_hal::read);
    RESET_FAKE(flash_hal::get_sector_size);
}

TEST(SettingsBackendTest, WriteErrorOn0)
{
    RESET_FAKE(flash_hal::read);
    RESET_FAKE(flash_hal::get_sector_size);
    RESET_FAKE(flash_hal::write);
    RESET_FAKE(flash_hal::erase);

    flash_hal::read_fake.custom_fake           = two_settings;
    flash_hal::get_sector_size_fake.return_val = SECTOR_SIZE;
    flash_hal::write_fake.custom_fake          = write_error_0;

    char str[] = "1";
    settings::save_setting((settings::ID)1, str);

    ASSERT_EQ(flash_hal::erase_fake.call_count, 1);
    ASSERT_GT(flash_hal::write_fake.call_count, 1);
}

TEST(SettingsBackendTest, WriteErrorOn1)
{
    RESET_FAKE(flash_hal::read);
    RESET_FAKE(flash_hal::get_sector_size);
    RESET_FAKE(flash_hal::write);
    RESET_FAKE(flash_hal::erase);

    flash_hal::read_fake.custom_fake           = two_settings;
    flash_hal::get_sector_size_fake.return_val = SECTOR_SIZE;
    flash_hal::write_fake.custom_fake          = write_error_1;

    char str[] = "1";
    settings::save_setting((settings::ID)1, str);

    ASSERT_EQ(flash_hal::erase_fake.call_count, 1);
    ASSERT_GT(flash_hal::write_fake.call_count, 1);
}

TEST(SettingsBackendTest, WriteErrorOn2)
{
    RESET_FAKE(flash_hal::read);
    RESET_FAKE(flash_hal::get_sector_size);
    RESET_FAKE(flash_hal::write);
    RESET_FAKE(flash_hal::erase);

    flash_hal::read_fake.custom_fake           = two_settings;
    flash_hal::get_sector_size_fake.return_val = SECTOR_SIZE;
    flash_hal::write_fake.custom_fake          = write_error_2;

    char str[] = "1";
    settings::save_setting((settings::ID)1, str);

    ASSERT_EQ(flash_hal::erase_fake.call_count, 1);
    ASSERT_GT(flash_hal::write_fake.call_count, 1);
}

TEST(SettingsBackendTest, WriteErrorNextSector)
{
    RESET_FAKE(flash_hal::read);
    RESET_FAKE(flash_hal::get_sector_size);
    RESET_FAKE(flash_hal::write);
    RESET_FAKE(flash_hal::erase);

    flash_hal::read_fake.custom_fake           = mostly_abandoned;
    flash_hal::get_sector_size_fake.return_val = SECTOR_SIZE;
    flash_hal::write_fake.custom_fake          = write_error_ns;

    char str[] = "1";
    settings::save_setting((settings::ID)1, str);

    ASSERT_GT(flash_hal::erase_fake.call_count, 1);
    ASSERT_GT(flash_hal::write_fake.call_count, 1);
}

TEST(SettingsBackendTest, WriteErrorOn0NextSector)
{
    RESET_FAKE(flash_hal::read);
    RESET_FAKE(flash_hal::get_sector_size);
    RESET_FAKE(flash_hal::write);
    RESET_FAKE(flash_hal::erase);

    flash_hal::read_fake.custom_fake           = mostly_abandoned;
    flash_hal::get_sector_size_fake.return_val = SECTOR_SIZE;
    flash_hal::write_fake.custom_fake          = write_error_0_ns;

    char str[] = "1";
    settings::save_setting((settings::ID)1, str);

    ASSERT_GT(flash_hal::erase_fake.call_count, 1);
    ASSERT_GT(flash_hal::write_fake.call_count, 1);
}

TEST(SettingsBackendTest, WriteErrorOn1NextSector)
{
    RESET_FAKE(flash_hal::read);
    RESET_FAKE(flash_hal::get_sector_size);
    RESET_FAKE(flash_hal::write);
    RESET_FAKE(flash_hal::erase);

    flash_hal::read_fake.custom_fake           = mostly_abandoned;
    flash_hal::get_sector_size_fake.return_val = SECTOR_SIZE;
    flash_hal::write_fake.custom_fake          = write_error_1_ns;

    char str[] = "1";
    settings::save_setting((settings::ID)1, str);

    ASSERT_GT(flash_hal::erase_fake.call_count, 1);
    ASSERT_GT(flash_hal::write_fake.call_count, 1);
}

TEST(SettingsBackendTest, WriteErrorOn2NextSector)
{
    RESET_FAKE(flash_hal::read);
    RESET_FAKE(flash_hal::get_sector_size);
    RESET_FAKE(flash_hal::write);
    RESET_FAKE(flash_hal::erase);

    flash_hal::read_fake.custom_fake           = mostly_abandoned;
    flash_hal::get_sector_size_fake.return_val = SECTOR_SIZE;
    flash_hal::write_fake.custom_fake          = write_error_2_ns;

    char str[] = "1";
    settings::save_setting((settings::ID)1, str);

    ASSERT_GT(flash_hal::erase_fake.call_count, 1);
    ASSERT_GT(flash_hal::write_fake.call_count, 1);
}

TEST(SettingsBackendTest, EraseErrorAllSectors)
{
    RESET_FAKE(flash_hal::read);
    RESET_FAKE(flash_hal::get_sector_size);
    RESET_FAKE(flash_hal::write);
    RESET_FAKE(flash_hal::erase);

    flash_hal::write_fake.custom_fake          = write_error_all;
    flash_hal::read_fake.custom_fake           = abandoned_sector;
    flash_hal::erase_fake.custom_fake          = erase_error_all_sectors;
    flash_hal::get_sector_size_fake.return_val = SECTOR_SIZE;

    char str[] = "1";
    settings::save_setting((settings::ID)0, str);

    ASSERT_GT(flash_hal::erase_fake.call_count, 1);
    ASSERT_GT(flash_hal::write_fake.call_count, 1);
}

TEST(SettingsBackendTest, EraseErrorFullLoop)
{
    RESET_FAKE(flash_hal::read);
    RESET_FAKE(flash_hal::get_sector_size);
    RESET_FAKE(flash_hal::write);
    RESET_FAKE(flash_hal::erase);

    flash_hal::write_fake.custom_fake          = write_error_all;
    flash_hal::read_fake.custom_fake           = mostly_abandoned;
    flash_hal::erase_fake.custom_fake          = erase_error_loop;
    flash_hal::get_sector_size_fake.return_val = SECTOR_SIZE;

    char str[] = "1";
    settings::save_setting((settings::ID)0, str);

    ASSERT_GT(flash_hal::erase_fake.call_count, 1);
    ASSERT_GT(flash_hal::write_fake.call_count, 1);
}

// End of File