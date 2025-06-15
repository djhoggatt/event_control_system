/// @file settings_test.hpp
/// Unit test accessor declarations for the settings module.

#ifndef SETTINGS_TEST_H
    #define SETTINGS_TEST_H

    #include "settings.hpp"

namespace settings_test
{
    //--------------------------------------------------------------------------
    //  Accessor Functions
    //--------------------------------------------------------------------------

    /// @brief Test get param declaration
    ///
    int32_t get_param(settings::ID setting, uintptr_t value);

    /// @brief Test set param declaration
    ///
    int32_t set_param(settings::ID setting, uintptr_t value, bool boot);

    /// @brief Get the current sector address for the flash backend.
    /// @return Current sector address
    ///
    uint32_t get_curr_sector();

    // End of Namespace
}

#endif

// End of File