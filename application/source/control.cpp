/// @file control.cpp
/// @author Denver Hoggatt
/// @brief Control definitions.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "control.hpp"
#include "error.hpp"
#include "macros.hpp"

#include <cstdint>
#include <cstring>

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


    //----------------------------------------------------------------------------------------------
    //  Private Function Prototypes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  File Variables
    //----------------------------------------------------------------------------------------------

    control::Control *controls[(uint32_t)control::ID::NumIDs];

#define DEF(CONTROL_NAME, CONTROL_STR, ENABLED) control::CONTROL_NAME CONTROL_NAME##_instance;
#include "controls.def"
#undef DEF

    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------


    // End of Anonymous Namespace
}

namespace control
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    char *get_list_of_controls()
    {
        constexpr uint32_t sep_len      = 2;
        constexpr uint32_t disabled_len = 8; // Len Disabled > Enabled
        constexpr uint32_t newline_len  = 2;
        constexpr uint32_t line_len     = MAX_NAME_LEN + sep_len + disabled_len + newline_len;

        static char ret_val[(line_len * (uint32_t)ID::NumIDs) + 1]; //+1 \0

        static char sep[sep_len + 1]           = ": ";
        static char enabled[disabled_len + 1]  = "enabled";
        static char disabled[disabled_len + 1] = "disabled";
        static char newline[newline_len + 1]   = "\r\n";

        ret_val[0] = '\0'; // Reset from last call
        for (uint32_t i = 0; i < (uint32_t)ID::NumIDs; i++)
        {
            strcat(ret_val, controls[i]->name);
            strcat(ret_val, sep);

            char *en_dis = controls[i]->enabled ? enabled : disabled;
            strcat(ret_val, en_dis);

            strcat(ret_val, newline);
        }

        return ret_val;
    }

    Control *get_control_by_name(const char *name)
    {
        REQUIRE(name, error::InvalidPointer);

        Control *ret_val = nullptr;

        for (uint32_t i = 0; i < (uint32_t)ID::NumIDs; i++)
        {
            uint32_t name_len = strlen(controls[i]->name);
            if (strncmp(name, controls[i]->name, name_len) == 0)
            {
                ret_val = controls[i];
                break;
            }
        }

        return ret_val;
    }

    void disperse_event(event::Event event)
    {
        REQUIRE(event.id < event::ID::NumEvents, error::InvalidID);

        for (uint32_t i = 0; i < (uint32_t)ID::NumIDs; i++)
        {
            if (!controls[i]->enabled)
            {
                continue;
            }

            HandleStatus status = controls[i]->handle_event(event);

            if (status != HandleStatus::NotHandled)
            {
                break;
            }
        }
    }

    void open()
    {
#define DEF(CONTROL_NAME, CONTROL_STR, ENABLED)                                  \
    controls[(uint32_t)ID::CONTROL_NAME]             = &CONTROL_NAME##_instance; \
    CONTROL_NAME##_instance.enabled                  = ENABLED;                  \
    static char CONTROL_NAME##_str[MAX_NAME_LEN + 1] = CONTROL_STR;              \
    CONTROL_NAME##_instance.name                     = CONTROL_NAME##_str;       \
    CONTROL_NAME##_instance.init_control();
#include "controls.def"
#undef DEF
    }

    int32_t get_param(settings::ID setting, uintptr_t value)
    {
        int32_t ret_val = (int32_t)error::NoError;

        for (uint32_t i = 0; i < (uint32_t)ID::NumIDs; i++)
        {
            int32_t status = controls[i]->get_param(setting, value);

            if (status != (int32_t)error::UnknownType)
            {
                ret_val = status;
                break;
            }
        }

        return (int32_t)ret_val;
    }

    int32_t set_param(settings::ID setting, uintptr_t value, bool bootup)
    {
        int32_t ret_val = (int32_t)error::NoError;

        for (uint32_t i = 0; i < (uint32_t)ID::NumIDs; i++)
        {
            int32_t status = controls[i]->set_param(setting, value, bootup);

            if (status != (int32_t)error::UnknownType)
            {
                ret_val = status;
                break;
            }
        }

        return (int32_t)ret_val;
    }

    //----------------------------------------------------------------------------------------------
    //  Class Function Definitions
    //----------------------------------------------------------------------------------------------

    int32_t Control::get_param(settings::ID setting, uintptr_t value)
    {
        UNUSED(setting);
        UNUSED(value);

        return (int32_t)error::UnknownType;
    }

    int32_t Control::set_param(settings::ID setting, uintptr_t value, bool bootup)
    {
        UNUSED(setting);
        UNUSED(value);
        UNUSED(bootup);

        return (int32_t)error::UnknownType;
    }

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

namespace control_test
{

    control::Control **get_controls()
    {
        return controls;
    }

    void override_control(uint32_t index, control::Control *test_control)
    {
        controls[index] = test_control;
    }

}

// End of File