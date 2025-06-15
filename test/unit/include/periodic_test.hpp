/// @file periodic_test.hpp
/// Unit test accessor declarations for the periodic module.

#ifndef PERIODIC_TEST_H
    #define PERIODIC_TEST_H

    #include "periodic.hpp"

namespace periodic_test
{
    //--------------------------------------------------------------------------
    //  Accessor Functions
    //--------------------------------------------------------------------------

    /// @brief Gets the period for the given periodic.
    /// @param id ID of the periodic.
    /// @return Period in milliseconds.
    ///
    uint32_t get_period(periodic::ID id);

    /// @brief Gets the enabled status for the given periodic.
    /// @param id ID of the periodic.
    /// @return True if enabled, otherwise false.
    ///
    bool get_enabled(periodic::ID id);

    // End of Namespace
}

#endif

// End of File