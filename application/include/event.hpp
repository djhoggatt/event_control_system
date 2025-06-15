/// @file event.hpp
/// @author Denver Hoggatt
/// @brief Event declarations.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#pragma once

#include "task.hpp"

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------


namespace event
{
    //----------------------------------------------------------------------------------------------
    //  Public Constants
    //----------------------------------------------------------------------------------------------

    /// @brief Size of the event queue. Must evenly divide 2^16.
    ///
    constexpr uint16_t QUEUE_SIZE = 256;

    //----------------------------------------------------------------------------------------------
    //  Public Data Types
    //----------------------------------------------------------------------------------------------

    enum class ID : uint32_t
    {
        NullEvent,

#define DEF(task_name, event_name) task_name##_##event_name,
#include "events.def"
#undef DEF

        NumEvents
    };

    struct Event
    {
            ID       id;
            task::ID task;
            void    *arg;
    };

    struct QueueInfo
    {
            uint32_t front_pos; // Queue front position
            uint32_t rear_pos;  // Queue rear position
    };

    //----------------------------------------------------------------------------------------------
    //  Classes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    task::ID get_associated_task(ID event_id);

    QueueInfo get_queue_info(task::ID task_id);

    void post(ID event, void *arg);

    Event handle(task::ID task_id);

    void init();

    // End of Namespace
}

// End of File