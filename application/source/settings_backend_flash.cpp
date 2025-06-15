/// @file settings_backend_flash.cpp
/// @author Denver Hoggatt
/// @brief Flash backend for settings
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "settings.hpp"
#include "flash_hal.hpp"
#include "error.hpp"

#include <cstdint>
#include <cstring>
#include <cstdio>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------

namespace
{
    //----------------------------------------------------------------------------------------------
    //  Private Constants
    //----------------------------------------------------------------------------------------------

    constexpr uint32_t MAX_SETTING_SIZE = 128;
    constexpr uint32_t NUM_SECTORS      = 2;

    constexpr uint32_t STARTING_OFFSET = 0;
    constexpr uint32_t UNSET_FLASH     = 0xFFFFFFFF;

    //----------------------------------------------------------------------------------------------
    //  Private Data Types
    //----------------------------------------------------------------------------------------------

    struct SettingFlashHeader
    {
            uint32_t id;
            uint32_t size;
            uint32_t deleted;
            uint32_t reserved;
    };

    //----------------------------------------------------------------------------------------------
    //  Private Function Prototypes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  File Variables
    //----------------------------------------------------------------------------------------------

    constexpr uint32_t header_size = sizeof(SettingFlashHeader);

    uint32_t curr_sector_addr;

    uint32_t fail_sector;
    bool     failt_sector_set = false;

    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------

    /// @brief Is the element empty?
    /// @param item Element to test.
    /// @return True if the element is unset flash.
    ///
    bool empty(uint32_t item)
    {
        return (item == UNSET_FLASH);
    }

    /// @brief Returns the ending offset for the given sector.
    /// @param sector_start Start of the sector.
    /// @return End of the sector.
    ///
    uint32_t sector_end(uint32_t sector_start)
    {
        return (sector_start + flash_hal::get_sector_size()) & ~(flash_hal::get_sector_size() - 1);
    }

    /// @brief Returns the address after the given entry.
    /// @param addr Address of the current entry.
    /// @param header Header data of the current entry.
    /// @return Address after the current entry.
    ///
    uint32_t increment(uint32_t addr, SettingFlashHeader header)
    {
        return flash_hal::align(addr + header_size + header.size);
    }

    /// @brief Finds the next non-deleted entry in the sector.
    /// @param starting_addr Starting address to search from.
    /// @return Address of the next entry. UINT32_MAX if no entry is found.
    ///
    uint32_t next_entry(uint32_t starting_addr)
    {
        uint32_t next_addr = UINT32_MAX;

        SettingFlashHeader header = { 0, 0, 0, UNSET_FLASH };
        uint32_t           i      = starting_addr;
        for (; i < sector_end(starting_addr); i = increment(i, header))
        {
            flash_hal::read(i, (uint8_t *)(&header), header_size);

            if (empty(header.id))
            {
                break; // Reached end of settings
            }

            if (empty(header.deleted))
            {
                next_addr = i;
                break;
            }
        }

        return next_addr;
    }

    /// @brief Finds the next empty address in the sector.
    /// @param starting_addr Starting address to search from.
    /// @return Empty address. UINT32_MAX if no empty address is found.
    ///
    uint32_t next_empty_addr(uint32_t starting_addr)
    {
        uint32_t next_addr = UINT32_MAX;

        SettingFlashHeader header = { 0, 0, 0, UNSET_FLASH };
        uint32_t           i      = starting_addr;
        for (; i < sector_end(starting_addr); i = increment(i, header))
        {
            flash_hal::read(i, (uint8_t *)(&header), header_size);
            if (empty(header.id))
            {
                next_addr = i;
                break;
            }
        }

        return next_addr;
    }

    /// @brief Copies all non-deleted entries from the old sector to the new sector, and switches to
    /// the next sector.
    ///
    void next_sector()
    {
        uint32_t curr_sector     = curr_sector_addr / flash_hal::get_sector_size();
        uint32_t old_sector_addr = curr_sector_addr;

        curr_sector++;
        if (curr_sector < NUM_SECTORS)
        {
            curr_sector_addr = curr_sector * flash_hal::get_sector_size();
        }
        else
        {
            curr_sector_addr = STARTING_OFFSET;
        }

        if (!failt_sector_set)
        {
            failt_sector_set = true;
            fail_sector      = curr_sector; // Sector that you are trying first
        }

        while (flash_hal::erase(curr_sector_addr) != error::NoError)
        {
            curr_sector++;

            // Check if you've looped through all sectors
            if (curr_sector == fail_sector)
            {
                printf("Flash Erase/Write Error: All sectors failed\r\n");
                return;
            }

            if (curr_sector < NUM_SECTORS)
            {
                curr_sector_addr = curr_sector * flash_hal::get_sector_size();
            }
            else
            {
                curr_sector      = 0;
                curr_sector_addr = STARTING_OFFSET;
            }
        }

        uint32_t           curr_sector_ptr = curr_sector_addr;
        SettingFlashHeader header          = { 0, 0, 0, UNSET_FLASH };
        uint32_t           i               = next_entry(old_sector_addr);
        for (; i != UINT32_MAX; i = next_entry(increment(i, header)))
        {
            flash_hal::read(i, (uint8_t *)(&header), sizeof(header));

            uint8_t buf[MAX_SETTING_SIZE];
            flash_hal::read(i + sizeof(header), buf, header.size);

            if (flash_hal::write(
                    curr_sector_addr + curr_sector_ptr, (uint8_t *)(&header), sizeof(header))
                != error::NoError)
            {
                next_sector();
                return;
            }

            if (flash_hal::write(
                    curr_sector_addr + curr_sector_ptr + sizeof(header), buf, header.size)
                != error::NoError)
            {
                next_sector();
                return;
            }
            curr_sector_ptr += sizeof(header) + header.size;

            header.deleted = 0;
            if (flash_hal::write(i, (uint8_t *)(&header), sizeof(header)) != error::NoError)
            {
                next_sector();
                return;
            }
        }

        failt_sector_set = false;
    }

    // End of Anonymous Namespace
}

namespace settings
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    /// @brief Saves the given setting.
    /// @param id ID of the setting.
    /// @param value Value of the setting.
    /// @return No error on success.
    ///
    error::Error save_setting(ID id, char *value)
    {
        REQUIRE(id < ID::NumSettings, error::InvalidID);
        REQUIRE(value != nullptr, error::InvalidPointer);
        REQUIRE(strlen(value) < MAX_SETTING_SIZE, error::InvalidLength);

        error::Error ret_val = error::NoError;

        uint32_t next_addr      = next_empty_addr(curr_sector_addr);
        uint32_t next_next_addr = next_addr + sizeof(SettingFlashHeader) + strlen(value);

        bool no_empty_addrs = (UINT32_MAX == next_addr);
        bool past_sector_bounds
            = next_next_addr > (curr_sector_addr + flash_hal::get_sector_size());
        if (no_empty_addrs || past_sector_bounds)
        {
            next_sector(); // Sector full, consolidate to next sector
        }

        // Delete any existing settings with the same ID
        SettingFlashHeader header = { 0, 0, 0, UNSET_FLASH };
        uint32_t           i      = next_entry(curr_sector_addr);
        for (; i != UINT32_MAX; i = next_entry(increment(i, header)))
        {
            flash_hal::read(i, (uint8_t *)(&header), sizeof(header));

            if (header.id == (uint32_t)id)
            {
                header.deleted = 0;
                if (flash_hal::write(i, (uint8_t *)(&header), sizeof(header)) != error::NoError)
                {
                    next_sector();
                    save_setting(id, value);
                    return ret_val;
                }
            }
        }

        uint8_t buf[MAX_SETTING_SIZE];
        memset(buf, 0xFF, MAX_SETTING_SIZE);
        memcpy(buf, value, strlen(value));

        header = { (uint32_t)id, (uint32_t)strlen(value), UNSET_FLASH, UNSET_FLASH };

        if (flash_hal::write(next_addr, (uint8_t *)(&header), sizeof(header)) != error::NoError)
        {
            next_sector();
        }

        if (flash_hal::write(next_addr + sizeof(header), (uint8_t *)buf, header.size)
            != error::NoError)
        {
            next_sector();
        }

        return ret_val;
    }

    /// @brief Loads all settings from flash.
    /// @return No error on success.
    ///
    error::Error load_settings()
    {
        error::Error ret_val = error::NoError;

        SettingFlashHeader header = { 0, 0, 0, UNSET_FLASH };
        uint32_t           i      = next_entry(curr_sector_addr);
        for (; i != UINT32_MAX; i = next_entry(increment(i, header)))
        {
            flash_hal::read(i, (uint8_t *)(&header), sizeof(header));

            INVAR(header.size < MAX_SETTING_SIZE, error::ReadFailed);

            uint8_t buf[MAX_SETTING_SIZE];
            flash_hal::read(i + sizeof(header), buf, header.size);

            // Must not save to prevent overwriting existing entry
            set((ID)header.id, (char *)buf, false);
        }

        return ret_val;
    }

    /// @brief Initializes this settings backend.
    ///
    void backend_init()
    {
        curr_sector_addr = 0; // Use this default if no entries in flash

        uint32_t i          = STARTING_OFFSET;
        uint32_t flash_size = flash_hal::get_sector_size() * NUM_SECTORS;
        for (; i < flash_size; i = sector_end(i))
        {
            if ((next_entry(i) != UINT32_MAX) || (next_empty_addr(i) != UINT32_MAX))
            {
                curr_sector_addr = i;
                break;
            }
        }
    }

    //----------------------------------------------------------------------------------------------
    //  Class Function Definitions
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Class Operator Definitions
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Class Constructor Definitions
    //----------------------------------------------------------------------------------------------


    // End of Namespace
}

//--------------------------------------------------------------------------------------------------
// Global Namespace Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Unit Test Accessors
//--------------------------------------------------------------------------------------------------

namespace settings_test
{

    uint32_t get_curr_sector()
    {
        return curr_sector_addr;
    }

}

// End of File