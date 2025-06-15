/// @file control.hpp
/// @author Denver Hoggatt
/// @brief Control declarations
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#pragma once

#include "input.hpp"
#include "output.hpp"
#include "io.hpp"
#include "event.hpp"

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------


namespace control
{
    //----------------------------------------------------------------------------------------------
    //  Public Constants
    //----------------------------------------------------------------------------------------------

    constexpr uint32_t MAX_NAME_LEN = 64;
    constexpr uint32_t CMD_STR_LEN  = 128;

    //----------------------------------------------------------------------------------------------
    //  Public Data Types
    //----------------------------------------------------------------------------------------------

    enum class HandleStatus : uint32_t
    {
        Handled,
        NotHandled,
    };

    enum class ID : uint32_t
    {
#define DEF(CONTROL_NAME, CONTROL_STR, ENABLED) CONTROL_NAME,
#include "controls.def"
#undef DEF

        NumIDs
    };

    //----------------------------------------------------------------------------------------------
    //  Classes
    //----------------------------------------------------------------------------------------------

    class Control
    {
        public:
            // -----------------------------------------------------------------
            //  Class Public Variables
            // -----------------------------------------------------------------

            /// @brief True if the control is enabled, otherwise false.
            ///
            bool enabled;

            /// @brief Name of the control.
            ///
            const char *name;

            // -----------------------------------------------------------------
            //  Class Public Functions
            // -----------------------------------------------------------------

            /// @brief Control specific get param.
            /// @param setting Setting to get.
            /// @param value Pointer that will be set with the current value.
            /// @return Unknown type if the setting is not relevant to this
            /// control, otherwise no error.
            ///
            virtual int32_t get_param(settings::ID setting, uintptr_t value);

            /// @brief Control specific set param.
            /// @param setting Setting to set.
            /// @param value Value that will be set.
            /// @param bootup True if booting up.
            /// @return Unknown type if the setting is not relevant to this
            /// control, otherwise no error.
            ///
            virtual int32_t set_param(settings::ID setting, uintptr_t value, bool bootup);

            /// @brief Event handler for CLI.
            /// @param evt Event to handle.
            /// @return Handled to prevent further processing of the event, otherwise NotHandled.
            ///
            virtual HandleStatus handle_event(event::Event evt) = 0;

            /// @brief Initializes the control.
            ///
            virtual void init_control() = 0;

            // End of Class
    };

#define DEF(CONTROL_NAME, CONTROL_STR, ENABLED)          \
    class CONTROL_NAME : public Control                  \
    {                                                    \
        public:                                          \
            HandleStatus handle_event(event::Event evt); \
            void         init_control();                 \
    };
#include "controls.def"
#undef DEF

    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    char *get_list_of_controls();

    Control *get_control_by_name(const char *name);

    void disperse_event(event::Event event);

    void open();

    int32_t get_param(settings::ID setting, uintptr_t value);

    int32_t set_param(settings::ID setting, uintptr_t value, bool bootup);

    // End of Namespace
}

// End of File