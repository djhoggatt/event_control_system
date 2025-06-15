/// @file task_control.cpp
/// Definitions for Control handling task.

#include "task_control.hpp"
#include "task.hpp"
#include "event.hpp"
#include "error.hpp"
#include "input.hpp"
#include "control.hpp"
#include "adc_hal.hpp"
#include "macros.hpp"

#include <inttypes.h>

//------------------------------------------------------------------------------
//  Macros and Error Checking
//------------------------------------------------------------------------------


namespace task_control
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


        //---------------------------------------------------------------------
        //  Private Functions
        //---------------------------------------------------------------------

        /// @brief Handles events for the control task.
        /// @param task_id ID of the control task.
        ///
        void handle_events(task::ID task_id)
        {
            ENSURE(task_id == task::ID::control, error::InvalidID);

            event::Event event = event::handle(task_id);
            while (event.id != event::ID::NullEvent)
            {
                control::disperse_event(event);

                event = event::handle(task_id);
            }
        }

        /// @brief Opens any task specific modules or sets task specific
        /// open-time variables.
        ///
        void open_modules()
        {
            // Currently this isn't opening anything, but might in the future.
        }

        // End of Anonymous Namespace
    }

    //-------------------------------------------------------------------------
    //  Public Functions
    //-------------------------------------------------------------------------

    void task_func(void *argument)
    {
        UNUSED(argument);
        task::wait_strict(task::Signal::GlobalOpen);

        // Open Time
        open_modules();
        task::send_open_signal(task_func);
        task::wait_strict(task::Signal::GlobalRun);

        // Run Time
        task::ID task_id = task::get_id(task_func);
        while (1)
        {
            uint32_t rcvd_signals = task::wait_any();

            uint32_t event_sig = static_cast<uint32_t>(task::Signal::GlobalEvent);
            if (rcvd_signals & event_sig)
            {
                handle_events(task_id);
            }

            if (rcvd_signals & (uint32_t)task::Signal::GlobalTerminate)
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


//------------------------------------------------------------------------------
// Unit Test Accessors
//------------------------------------------------------------------------------

namespace task_control_test
{
}

// End of File