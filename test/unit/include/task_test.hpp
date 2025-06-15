/// @file task_test.hpp
/// Unit test accessor declarations for the task module.

#ifndef TASK_TEST_H
    #define TASK_TEST_H

    #include "task.hpp"

namespace task_test
{
    //--------------------------------------------------------------------------
    //  Accessor Functions
    //--------------------------------------------------------------------------

    /// @brief Gets the handle from the associated task.
    /// @param id ID of the task.
    /// @return Handle of the task.
    ///
    void *get_handle_from_id(task::ID id);

    /// @brief Sets the handle of the associated task.
    /// @param id ID of the task.
    /// @param handle Handle of the task.
    ///
    void set_handle_by_id(task::ID id, void *handle);

    /// @brief The open signal doesn't seem to persist through tests, and gets
    /// reset after  initialization. This is a workaround, to allow us to
    /// directly set the signal.
    /// @param id ID of the task.
    /// @param sig Signal to set.
    ///
    void set_open_sig_by_id(task::ID id, uint32_t sig);

    // End of Namespace
}

#endif

// End of File