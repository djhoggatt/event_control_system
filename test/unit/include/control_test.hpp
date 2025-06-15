/// @file control_test.hpp
/// Unit test accessor declarations for the control module.

#ifndef CONTROL_TEST_H
    #define CONTROL_TEST_H

    #include "control.hpp"

namespace control_test
{
    //--------------------------------------------------------------------------
    //  Accessor Functions
    //--------------------------------------------------------------------------

    /// @brief Gets the internal controls list.
    /// @return Control list.
    ///
    control::Control **get_controls();

    /// @brief Puts the given control in the control list at the given index.
    /// @param index Index where the control is overriden.
    /// @param test_control Control that will be used instead.
    ///
    void override_control(uint32_t index, control::Control *test_control);

    // End of Namespace
}

#endif

// End of File