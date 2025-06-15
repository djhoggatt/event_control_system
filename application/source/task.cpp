/// @file task.cpp
/// @author Denver Hoggatt
/// @brief Task definitions
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "task.hpp"
#include "task_osal.hpp"
#include "error.hpp"
#include "task_open.hpp"
#include "task_control.hpp"

#include <cstring>
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

    enum class TaskPriority : uint32_t
    {
        Lowest,
        Low,
        Medium,
        High,
        Highest,
    };

    struct Task
    {
            const task::ID     id;
            const TaskPriority priority;
            const uint16_t     stack_depth;
            const task::Func   func;
            uint32_t           stack_base;

            void *handle;

            uint32_t open_signal;
    };

    //----------------------------------------------------------------------------------------------
    //  Private Function Prototypes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  File Variables
    //----------------------------------------------------------------------------------------------

    Task tasks[] = {

#define DEF(task_name, prior, depth)                \
    {                                               \
        .id          = task::ID::task_name,         \
        .priority    = TaskPriority::prior,         \
        .stack_depth = depth,                       \
        .func        = task_##task_name::task_func, \
        .stack_base  = 0,                           \
        .handle      = nullptr,                     \
        .open_signal = 0,                           \
    },
#include "tasks.def"
#undef DEF

    };

    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------

    /// @brief Gets a pointer to a task using the given id.
    /// @param id ID of the task.
    /// @return Pointer to the task.
    ///
    Task *get_task_by_id(task::ID id)
    {
        Task *ret_val = nullptr;

        for (uint32_t i = 0; i < task::num(); i++)
        {
            if (tasks[i].id == id)
            {
                ret_val = &tasks[i];
            }
        }

        return ret_val;
    }

    /// @brief Gets a pointer to a task using the given function.
    /// @param func task_func of the task to get.
    /// @return Pointer to the task.
    ///
    Task *get_task_by_func(task::Func func)
    {
        Task *ret_val = nullptr;

        for (uint32_t i = 0; i < task::num(); i++)
        {
            if (tasks[i].func == func)
            {
                ret_val = &tasks[i];
            }
        }

        return ret_val;
    }

    /// @brief Shortcut to get the open task.
    /// @return Pointer to the open task.
    ///
    const Task *get_open_task()
    {
        Task *ret_val = get_task_by_id(task::ID::open);

        ENSURE(ret_val != nullptr, error::IDNotFound);

        return ret_val;
    }

    /// @brief Dumps the stack.
    /// @param stack_start Start of the stack.
    /// @param stack_size Size of the stack.
    ///
    void dump_stack(uint8_t *stack_start, uint32_t stack_size)
    {
        printf("Stack Dump:\r\n");

        for (uint32_t i = 0; i < stack_size; i++)
        {
            printf("%02X ", stack_start[i]);

            if ((i + 1) % 16 == 0)
            {
                printf("\r\n");
            }
        }

        printf("\r\n");
    }

    // End of Anonymous Namespace
}

namespace task
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    /// @brief Prints the maximum stack usage statistics.
    /// @param dump True to also print out the stack contents.
    ///
    void print_maximum_stack_usage(bool dump)
    {
        for (uint32_t i = 0; i < num(); i++)
        {
            task_osal::StackInfo info = task_osal::get_stack_info(tasks[i].id);

            printf("Task %" PRIu32 " Stack Usage:\r\n", (uint32_t)tasks[i].id);
            printf("Stack %" PRIu32 " Base           (addr): %p\r\n",
                   (uint32_t)tasks[i].id,
                   info.base);
            printf("Stack %" PRIu32 " End            (addr): %p\r\n",
                   (uint32_t)tasks[i].id,
                   &info.base[info.size]);
            printf("Stack %" PRIu32 " Size          (bytes): %" PRIu32 "\r\n",
                   (uint32_t)tasks[i].id,
                   info.size);
            printf("Stack %" PRIu32 " Maximum Usage (bytes): %" PRIu32 "\r\n",
                   (uint32_t)tasks[i].id,
                   info.high_water);
            printf("\r\n");

            if (dump)
            {
                dump_stack(info.base, info.size);
            }
        }
    }

    /// @brief Get the number of tasks.
    /// @return Number of tasks.
    ///
    uint32_t num()
    {
        return static_cast<uint32_t>(ID::NumIDs);
    }

    /// @brief Gets a task ID using the task_func of a task.
    /// @param func task_func of the task.
    /// @return Task ID.
    ///
    ID get_id(Func func)
    {
        REQUIRE(func != nullptr, error::InvalidPointer);

        ID ret_val = ID::NumIDs;

        Task *task = get_task_by_func(func);
        if (task != nullptr)
        {
            ret_val = task->id;
        }

        return ret_val;
    }

    /// @brief Sends the open signal for the associated task.
    /// @param calling_func task_func of the task.
    ///
    void send_open_signal(const Func calling_func)
    {
        REQUIRE(calling_func != nullptr, error::InvalidPointer);

        Task *task = get_task_by_func(calling_func);
        if (task != nullptr)
        {
            const Task *open_task = get_open_task();
            task_osal::send_signal(open_task->handle, task->open_signal);
        }
    }

    /// @brief Sends a signal to another task.
    /// @param task_id ID of the task to signal.
    /// @param signal Signal to send.
    ///
    void send_signal(ID task_id, Signal signal)
    {
        REQUIRE(task_id < ID::NumIDs, error::InvalidID);
        REQUIRE(signal < Signal::NumSigs, error::InvalidSignal);

        Task *task = get_task_by_id(task_id);
        if (task != nullptr)
        {
            task_osal::send_signal(task->handle, static_cast<uint32_t>(signal));
        }
    }

    /// @brief Broadcast a signal to all tasks.
    /// @param signal Signal to broadcast.
    /// @param calling_func task_func of the task that is broadcasting.
    ///
    void broadcast(Signal signal, const Func calling_func)
    {
        REQUIRE(signal < Signal::NumSigs, error::InvalidID);

        ID thisTask = get_id(calling_func);

        for (uint32_t i = 0; i < num(); i++)
        {
            if (tasks[i].id != thisTask)
            {
                task_osal::send_signal(tasks[i].handle, static_cast<uint32_t>(signal));
            }
        }
    }

    /// @brief Waits for a specific signal.
    /// @param signal Signal to wait for.
    ///
    void wait_strict(Signal signal)
    {
        REQUIRE(signal < Signal::NumSigs, error::InvalidID);

        uint32_t rcvd_sig     = task_osal::wait_signal();
        uint32_t expected_sig = static_cast<uint32_t>(signal);
        while ((rcvd_sig & expected_sig) == 0)
        {
            rcvd_sig = task_osal::wait_signal();
        }
    }

    /// @brief Waits for any signal.
    /// @return Received signal.
    ///
    uint32_t wait_any()
    {
        return task_osal::wait_signal();
    }

    /// @brief Initializes tasks.
    ///
    void init()
    {
        static bool inited = false;

        REQUIRE(!inited, error::TooManyAttempts);

        for (uint32_t i = 0; i < num(); i++)
        {
            tasks[i].open_signal = 1 << i;
            tasks[i].handle      = nullptr;

            error::Error err = task_osal::create_task(tasks[i].func,
                                                      static_cast<uint32_t>(tasks[i].id),
                                                      static_cast<uint16_t>(tasks[i].stack_depth),
                                                      static_cast<uint32_t>(tasks[i].priority),
                                                      &tasks[i].handle);

            ENSURE(err == error::NoError, err);
        }

        inited = true;
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

namespace task_test
{

    void *get_handle_from_id(task::ID id)
    {
        Task *task = get_task_by_id(id);
        return task->handle;
    }

    void set_handle_by_id(task::ID id, void *handle)
    {
        Task *task   = get_task_by_id(id);
        task->handle = handle;
    }

    void set_open_sig_by_id(task::ID id, uint32_t sig)
    {
        Task *task        = get_task_by_id(id);
        task->open_signal = sig;
    }

}

// End of File