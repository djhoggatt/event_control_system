/// @file settings.hpp
/// Declarations for the settings.

#ifndef SETTINGS_BACKEND_H
    #define SETTINGS_BACKEND_H

    #include "error.hpp"
    #include "settings.hpp"

    #include <cstdint>

//------------------------------------------------------------------------------
//  Macros and Error Checking
//------------------------------------------------------------------------------


namespace settings
{
    //--------------------------------------------------------------------------
    //  Public Constants
    //--------------------------------------------------------------------------


    //--------------------------------------------------------------------------
    //  Public Data Types
    //--------------------------------------------------------------------------


    //--------------------------------------------------------------------------
    //  Classes
    //--------------------------------------------------------------------------


    //--------------------------------------------------------------------------
    //  Public Functions
    //--------------------------------------------------------------------------

    /// @brief Saves the given setting to flash.
    /// @param id ID of the setting to save
    /// @param value Value, as a string, of the setting to save.
    /// @return No error on success.
    ///
    error::Error save_setting(ID id, char *value);

    /// @brief Loops through the flash and calls set on all non-deleted
    /// settings.
    /// @return No error on success.
    ///
    error::Error load_settings();

    /// @brief Initializes the settings backend.
    ///
    void backend_init();

    // End of Namespace
}

#endif

// End of File