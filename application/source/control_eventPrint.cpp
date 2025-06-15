/// @file control_eventPrint.cpp
/// @author Denver Hoggatt
/// @brief Control to print out the stream of events as they happen.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "control.hpp"
#include "event.hpp"
#include "uart.hpp"
#include "macros.hpp"

#include <cstdint>
#include <cinttypes>

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

    uart::UART *console;

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


    //----------------------------------------------------------------------------------------------
    //  Class Function Definitions
    //----------------------------------------------------------------------------------------------

    HandleStatus EvtPrint::handle_event(event::Event evt)
    {
        constexpr uint32_t MAX_STR_SIZE = 128;

        HandleStatus ret_val = HandleStatus::NotHandled;

        event::QueueInfo info = event::get_queue_info(event::get_associated_task(evt.id));

        char str[MAX_STR_SIZE];
        snprintf(str,
                 MAX_STR_SIZE,
                 "evt id:%" PRIu32 ", arg:0x%p, %" PRIu32 ", size:%" PRIu32 "\r\n",
                 (uint32_t)evt.id,
                 evt.arg,
                 (uint32_t)evt.task,
                 info.rear_pos - info.front_pos + 1);
        console->set<const char *>(str);

        return ret_val;
    }

    void EvtPrint::init_control()
    {
        console = dynamic_cast<uart::UART *>(output::get_by_id(io::IOID::UART_CONSOLE));
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


}

// End of File