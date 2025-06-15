/// @file event.cpp
/// @author Denver Hoggatt
/// @brief Event handling definitions.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "event.hpp"
#include "error.hpp"
#include "mutex.hpp"

#include <cstdint>
#include <atomic>

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

    __attribute__((section(".events"))) event::Event
        task_queues[static_cast<uint32_t>(task::ID::NumIDs)][event::QUEUE_SIZE];

    std::atomic_uint_fast16_t queue_rears[static_cast<int32_t>(task::ID::NumIDs)];
    std::atomic_uint_fast16_t queue_fronts[static_cast<int32_t>(task::ID::NumIDs)];

    task::ID event_task_assoc[static_cast<uint32_t>(event::ID::NumEvents)];

    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------


    // End of Anonymous Namespace
}

namespace event
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    /// @brief Returns the task ID associated with the event.
    /// @param event_id Event
    /// @return Associated task.
    ///
    task::ID get_associated_task(ID event_id)
    {
        return event_task_assoc[(uint32_t)event_id];
    }

    /// @brief Returns the front and rear position of the queue.
    /// @param task_id Task associated with the queue.
    /// @return Queue info.
    ///
    QueueInfo get_queue_info(task::ID task_id)
    {
        QueueInfo ret_val;

        ret_val.front_pos = queue_fronts[(uint32_t)task_id].load() % QUEUE_SIZE;
        ret_val.rear_pos  = queue_rears[(uint32_t)task_id].load() % QUEUE_SIZE;

        return ret_val;
    }

    /// @brief Post the event.
    /// @param event_id ID of the event to post.
    /// @param arg "Thin" (pointer length) argument associated with the event.
    ///
    void post(ID event_id, void *arg)
    {
        REQUIRE(event_id < ID::NumEvents, error::InvalidID);

        const uint32_t id      = static_cast<uint32_t>(event_id);
        uint32_t       task_id = static_cast<uint32_t>(event_task_assoc[id]);

        volatile uint16_t pos      = queue_rears[task_id].fetch_add(1) % QUEUE_SIZE;
        volatile uint16_t next_pos = (pos + 1) % QUEUE_SIZE;
        volatile uint16_t front    = queue_fronts[task_id].load() % QUEUE_SIZE;

        INVAR(next_pos != front, error::QueueOverflow);

        std::atomic<uint32_t> memory_barrier;
        memory_barrier.load();

        volatile Event *event = &task_queues[task_id][pos];
        event->id             = event_id;
        event->arg            = arg;

        task::send_signal(event_task_assoc[id], task::Signal::GlobalEvent);
    }

    /// @brief Event handler. This should be called by the associated task to receive events sent to
    /// that task.
    /// @param task_id ID of the task.
    /// @return Event at the top of the queue.
    ///
    Event handle(task::ID task_id)
    {
        REQUIRE(task_id < task::ID::NumIDs, error::IDNotFound);

        Event ret_val;
        ret_val.id   = ID::NullEvent;
        ret_val.task = task::ID::NumIDs;
        ret_val.arg  = nullptr;

        mutex::take(mutex::ID::EventHandle);

        uint32_t id    = static_cast<uint32_t>(task_id);
        uint16_t front = queue_fronts[id].load() % QUEUE_SIZE;
        uint16_t rear  = queue_rears[id].load() % QUEUE_SIZE;
        if (front != rear)
        {
            ret_val = task_queues[id][front];
            queue_fronts[id]++;
        }

        mutex::give(mutex::ID::EventHandle);

        ENSURE(ret_val.id < ID::NumEvents, error::InvalidID);
        ENSURE(ret_val.task <= task::ID::NumIDs, error::OperationFail);

        return ret_val;
    }

    /// @brief Initializes the event module.
    ///
    void init()
    {
        REQUIRE(std::atomic_is_lock_free(queue_rears), error::DeviceInitFailed);
        REQUIRE(std::atomic_is_lock_free(queue_fronts), error::DeviceInitFailed);

        const uint32_t num_tasks = static_cast<uint32_t>(task::ID::NumIDs);
        for (uint32_t task = 0; task < num_tasks; task++)
        {
            queue_fronts[task] = 0;
            queue_rears[task]  = 0;

            for (uint32_t i = 0; i < QUEUE_SIZE; i++)
            {
                task_queues[task][i].task = static_cast<task::ID>(task);
            }
        }

#define DEF(task_name, event_name)                                                      \
    const uint32_t event_name    = static_cast<uint32_t>(ID::task_name##_##event_name); \
    event_task_assoc[event_name] = task::ID::task_name;
#include "events.def"
#undef DEF
    }

    //----------------------------------------------------------------------------------------------
    //  Class Function Definitions
    //----------------------------------------------------------------------------------------------


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

namespace event_test
{


}

// End of File