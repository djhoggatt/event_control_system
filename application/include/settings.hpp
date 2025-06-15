/// @file settings.hpp
/// @author Denver Hoggatt
/// @brief Settings declarations
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#pragma once

#include <cstdint>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------


namespace settings
{
    //----------------------------------------------------------------------------------------------
    //  Public Constants
    //----------------------------------------------------------------------------------------------

    constexpr uint32_t MAX_STR_LEN = 64;

    //----------------------------------------------------------------------------------------------
    //  Public Data Types
    //----------------------------------------------------------------------------------------------

    enum class ID : uint32_t
    {
#define DEF(NAME, MODULE, TYPE, PERMISSION, DEFAULT)       NAME,
#define DEF_FLOAT(NAME, MODULE, TYPE, PERMISSION, DEFAULT) NAME,
#include "settings.def"
#undef DEF
#undef DEF_FLOAT

        NumSettings
    };

    enum class Type : uint32_t
    {
        INT,
        UINT,
        HEX,
        STR,
        FLOAT,

        NumTypes
    };

    enum class Permission : uint32_t
    {
        SET,
        GET,
        SET_GET,

        NumPermissions
    };

    typedef int32_t (*GetParam)(ID, uintptr_t);
    typedef int32_t (*SetParam)(ID, uintptr_t, bool);

    struct Setting
    {
            ID         id;
            Type       type;
            GetParam   get_param;
            SetParam   set_param;
            Permission permission;
            char      *str_ptr;

            float float_val; // Floats can't be passed as uintptr_t, so they
                             // must be stored and then passed as a pointer.
    };

    //----------------------------------------------------------------------------------------------
    //  Classes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    int32_t set(ID id, const char *value, bool save);

    int32_t get(ID id, char *value);

    void init();

    // End of Namespace
}

// End of File