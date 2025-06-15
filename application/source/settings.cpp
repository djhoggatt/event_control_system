/// @file settings.cpp
/// @author Denver Hoggatt
/// @brief Settings fornt-end definitions
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "settings.hpp"
#include "settings_backend.hpp"
#include "error.hpp"
#include "version.hpp"
#if defined(TESTING)
    #include "settings_test.hpp"
#endif
#include "control.hpp"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cinttypes>
#include <cstdio>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------

namespace
{
    //----------------------------------------------------------------------------------------------
    //  Private Constants
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Private Data Types
    //----------------------------------------------------------------------------------------------

#ifndef TESTING

    settings::Setting settings_list[(uint32_t)settings::ID::NumSettings] = {
    #define DEF_FLOAT(NAME, MODULE, TYPE, PERMISSION, DEFAULT)
    #define DEF(NAME, MODULE, TYPE, PERMISSION, DEFAULT)    \
        {                                                   \
            .id         = settings::ID::NAME,               \
            .type       = settings::Type::TYPE,             \
            .get_param  = MODULE::get_param,                \
            .set_param  = MODULE::set_param,                \
            .permission = settings::Permission::PERMISSION, \
            .str_ptr    = nullptr,                          \
            .float_val  = 0.0,                              \
        },
    #include "settings.def"
    #undef DEF
    #undef DEF_FLOAT

    #define DEF(NAME, MODULE, TYPE, PERMISSION, DEFAULT)
    #define DEF_FLOAT(NAME, MODULE, TYPE, PERMISSION, DEFAULT) \
        {                                                      \
            .id         = settings::ID::NAME,                  \
            .type       = settings::Type::TYPE,                \
            .get_param  = MODULE::get_param,                   \
            .set_param  = MODULE::set_param,                   \
            .permission = settings::Permission::PERMISSION,    \
            .str_ptr    = nullptr,                             \
            .float_val  = 0.0,                                 \
        },
    #include "settings.def"
    #undef DEF
    #undef DEF_FLOAT

#else

    settings::Setting settings_list[(uint32_t)settings::ID::NumSettings] = {
    #define DEF_FLOAT(NAME, MODULE, TYPE, PERMISSION, DEFAULT)
    #define DEF(NAME, MODULE, TYPE, PERMISSION, DEFAULT)    \
        {                                                   \
            .id         = settings::ID::NAME,               \
            .type       = settings::Type::TYPE,             \
            .get_param  = settings_test::get_param,         \
            .set_param  = settings_test::set_param,         \
            .permission = settings::Permission::PERMISSION, \
            .str_ptr    = nullptr,                          \
            .float_val  = 0.0,                              \
        },
    #include "settings.def"
    #undef DEF
    #undef DEF_FLOAT

    #define DEF(NAME, MODULE, TYPE, PERMISSION, DEFAULT)
    #define DEF_FLOAT(NAME, MODULE, TYPE, PERMISSION, DEFAULT) \
        {                                                      \
            .id         = settings::ID::NAME,                  \
            .type       = settings::Type::TYPE,                \
            .get_param  = MODULE::get_param,                   \
            .set_param  = MODULE::set_param,                   \
            .permission = settings::Permission::PERMISSION,    \
            .str_ptr    = nullptr,                             \
            .float_val  = 0.0,                                 \
        },
    #include "settings.def"
    #undef DEF
    #undef DEF_FLOAT

#endif
    };

    //----------------------------------------------------------------------------------------------
    //  Private Function Prototypes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  File Variables
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------

    /// @brief Gets the setting using the given ID.
    /// @param id ID of the setting.
    /// @return Pointer to the setting.
    ///
    settings::Setting *get_setting_from_id(settings::ID id)
    {
        settings::Setting *ret_val = nullptr;

        uint32_t num_settings = (uint32_t)settings::ID::NumSettings;
        for (uint32_t i = 0; i < num_settings; i++)
        {
            if (settings_list[i].id == id)
            {
                ret_val = &settings_list[i];
                break;
            }
        }

        return ret_val;
    }

    /// @brief Set the value of the setting.
    /// @param id ID of the setting.
    /// @param value Value to set.
    /// @return ID not found if the setting does not exist, otherwise the return from the associated
    /// set param.
    ///
    int32_t set_val(settings::ID id, uintptr_t value)
    {
        REQUIRE(id < settings::ID::NumSettings, error::IDNotFound);

        int32_t ret_val = (int32_t)error::IDNotFound;

        settings::Setting *setting = get_setting_from_id(id);
        if (setting != nullptr)
        {
            ret_val = setting->set_param(id, value, false);
        }

        return ret_val;
    }

    /// @brief Get the value of the setting.
    /// @param id ID of the setting.
    /// @param value Value set.
    /// @return ID not found if the setting does not exist, otherwise the return from the associated
    /// set param.
    ///
    int32_t get_val(settings::ID id, uintptr_t value)
    {
        REQUIRE(id < settings::ID::NumSettings, error::IDNotFound);

        int32_t ret_val = (int32_t)error::IDNotFound;

        settings::Setting *setting = get_setting_from_id(id);
        if (setting != nullptr)
        {
            ret_val = setting->get_param(id, value);
            ret_val = error::NoError;
        }

        return ret_val;
    }

    // End of Anonymous Namespace
}

namespace settings
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    /// @brief Sets the setting.
    /// @param id ID of the setting.
    /// @param value String value to set.
    /// @param save If true, saves to flash.
    /// @return No error on success.
    ///
    int32_t set(ID id, const char *value, bool save)
    {
        REQUIRE(id < ID::NumSettings, error::IDNotFound);
        REQUIRE(value != nullptr, error::InvalidPointer);

        Setting *setting = get_setting_from_id(id);
        REQUIRE(setting != nullptr, error::InvalidIndex);

        int32_t ret_val = (int32_t)error::NoError;

        if (setting->permission == Permission::GET)
        {
            return (int32_t)error::WriteFailed;
        }

        switch (setting->type)
        {
            case Type::INT:
                ret_val = set_val(id, (uintptr_t)strtol(value, NULL, 10));
                break;

            case Type::UINT:
                ret_val = set_val(id, (uintptr_t)strtoul(value, NULL, 10));
                break;

            case Type::HEX:
                ret_val = set_val(id, (uintptr_t)strtoul(value, NULL, 16));
                break;

            case Type::STR:
                INVAR(setting->str_ptr != nullptr, error::InitFailed);
                strncpy(setting->str_ptr, value, MAX_STR_LEN);
                ret_val = set_val(id, (uintptr_t)setting->str_ptr);
                break;

            case Type::FLOAT:
                setting->float_val = strtof(value, NULL);
                ret_val            = set_val(id, (uintptr_t)(&setting->float_val));
                break;

            default:
                INVAR(false, error::UnknownType);
        }

        if (save && (ret_val == (int32_t)error::NoError))
        {
            save_setting(id, (char *)value);
        }

        return ret_val;
    }

    /// @brief Gets the setting.
    /// @param id ID of the settings.
    /// @param value String value of the setting.
    /// @return No error on success.
    ///
    int32_t get(ID id, char *value)
    {
        REQUIRE(id < ID::NumSettings, error::IDNotFound);
        REQUIRE(value != nullptr, error::InvalidPointer);

        Setting *setting = get_setting_from_id(id);
        REQUIRE(setting != nullptr, error::InvalidIndex);

        if (setting->permission == Permission::SET)
        {
            return (int32_t)error::ReadFailed;
        }

        uintptr_t val       = 0;
        float     float_val = 0.0;
        memset(value, 0, MAX_STR_LEN);
        switch (setting->type)
        {
            case Type::INT:
                get_val(id, (uintptr_t)(&val));
                snprintf(value, MAX_STR_LEN, "%" PRIi32 "", (int32_t)val);
                break;

            case Type::UINT:
                get_val(id, (uintptr_t)(&val));
                snprintf(value, MAX_STR_LEN, "%" PRIu32 "", (uint32_t)val);
                break;

            case Type::HEX:
                get_val(id, (uintptr_t)(&val));
                snprintf(value, MAX_STR_LEN, "0x%" PRIX32 "", (uint32_t)val);
                break;

            case Type::STR:
                get_val(id, (uintptr_t)(&val));
                snprintf(value, MAX_STR_LEN, "%s", (char *)val);
                break;

            case Type::FLOAT:
                get_val(id, (uintptr_t)(&float_val));
                snprintf(value, MAX_STR_LEN, "%f", (double)float_val);
                break;

            default:
                INVAR(false, error::UnknownType);
        }

        return (int32_t)error::NoError;
    }

    /// @brief Initializes the settings.
    ///
    void init()
    {
#define DEF_FLOAT(NAME, MODULE, TYPE, PERMISSION, DEFAULT)
#define DEF(NAME, MODULE, TYPE, PERMISSION, DEFAULT)                                        \
    if ((Type::TYPE == Type::STR) && (Permission::PERMISSION != Permission::GET))           \
    {                                                                                       \
        static char NAME##_str[MAX_STR_LEN + 1];                                            \
        settings_list[(uint32_t)ID::NAME].str_ptr = NAME##_str;                             \
        strncpy(NAME##_str, (char *)DEFAULT, MAX_STR_LEN);                                  \
        settings_list[(uint32_t)ID::NAME].set_param(ID::NAME, (uintptr_t)NAME##_str, true); \
    }                                                                                       \
    else if (Permission::PERMISSION != Permission::GET)                                     \
    {                                                                                       \
        settings_list[(uint32_t)ID::NAME].set_param(ID::NAME, (uintptr_t)DEFAULT, true);    \
    }
#include "settings.def"
#undef DEF
#undef DEF_FLOAT

#define DEF(NAME, MODULE, TYPE, PERMISSION, DEFAULT)
#define DEF_FLOAT(NAME, MODULE, TYPE, PERMISSION, DEFAULT)                              \
    if (Permission::PERMISSION != Permission::GET)                                      \
    {                                                                                   \
        settings_list[(uint32_t)ID::NAME].float_val = DEFAULT;                          \
        settings_list[(uint32_t)ID::NAME].set_param(                                    \
            ID::NAME, (uintptr_t)(&settings_list[(uint32_t)ID::NAME].float_val), true); \
    }
#include "settings.def"
#undef DEF
#undef DEF_FLOAT

        backend_init();

        load_settings();
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


}

// End of File