/// @file task_open.cpp
/// Definitions for open task.

#include "task_open.hpp"
#include "setup.hpp"
#include "task.hpp"
#include "macros.hpp"

#include <cstdint>
#include <cstring>

//------------------------------------------------------------------------------
//  Macros and Error Checking
//------------------------------------------------------------------------------


namespace task_open
{
    namespace
    {
        //---------------------------------------------------------------------
        //  Private Data Types
        //---------------------------------------------------------------------


        //---------------------------------------------------------------------
        //  Private Function Prototypes
        //---------------------------------------------------------------------


        //---------------------------------------------------------------------
        //  File Variables
        //---------------------------------------------------------------------

        bool terminate = false;

        //---------------------------------------------------------------------
        //  Private Functions
        //---------------------------------------------------------------------

        /// @brief Sets all bits lower than the given number to 1.
        /// @param bits_to_set Will set all bits up to this number.
        /// @return Value with all bits set.
        ///
        static uint32_t set_lower_bits(uint32_t bits_to_set)
        {
            uint32_t ret_val = 0;

            bool valid = bits_to_set < sizeof(ret_val) * 8; // 8 bits in a byte

            for (uint32_t i = 0; valid && (i < bits_to_set); i++)
            {
                ret_val |= 1 << i;
            }

            return ret_val;
        }

        // End of Anonymous Namespace
    }

    //-------------------------------------------------------------------------
    //  Public Functions
    //-------------------------------------------------------------------------

    void task_func(void *argument)
    {
        UNUSED(argument);

        // Open Time
        setup::open();

        task::broadcast(task::Signal::GlobalOpen, task_func);

        uint32_t num_tasks     = task::num();
        uint32_t required_sigs = set_lower_bits(num_tasks);
        required_sigs &= ~(1); // Sig for this task, tasks.def has more info.
        while (required_sigs)
        {
            uint32_t received_sigs = task::wait_any();
            required_sigs &= ~(received_sigs);
        }

        task::broadcast(task::Signal::GlobalRun, task_func);

        // Run Time
        while (1)
        {
            // Should not wake again
            task::wait_strict(task::Signal::GlobalInvalid);

            if (terminate)
            {
                break;
            }
        }
    }

    //-------------------------------------------------------------------------
    //  Class Definitions
    //-------------------------------------------------------------------------


    // End of Namespace
}

//------------------------------------------------------------------------------
// Global Namespace Functions
//------------------------------------------------------------------------------

namespace task_open_test
{
    void terminate()
    {
        task_open::terminate = true;
    }
}

// End of File