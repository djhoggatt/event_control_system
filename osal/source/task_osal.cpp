/// @file task_osal.cpp
/// @author Denver Hoggatt
/// @brief Task OSAL definitions
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "task_osal.hpp"

#include <cstdio>
#include <cstring>

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

    task_osal::TaskOSAL *task_osals[(uint32_t)osal::ID::NumIDs];

    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------

    void init()
    {
        static bool inited = false;
        if (inited)
        {
            return;
        }

#define DEF_RTOS(NAME) task_osals[(uint32_t)osal::ID::NAME] = task_osal::NAME##_get_funcs();
#include "rtos.def"
#undef DEF_RTOS

        inited = true;
    }

    // End of Anonymous Namespace
}

namespace task_osal
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    StackInfo get_stack_info(task::ID id)
    {
        init();

        StackInfo ret_val;
        memset(&ret_val, 0, sizeof(StackInfo));
        if (task_osals[(uint32_t)osal::rtos()] == nullptr)
        {
            return ret_val;
        }

        ret_val = task_osals[(uint32_t)osal::rtos()]->get_stack_info(id);

        return ret_val;
    }

    void send_signal(void *handle, uint32_t signal)
    {
        init();

        if (task_osals[(uint32_t)osal::rtos()] == nullptr)
        {
            return;
        }

        task_osals[(uint32_t)osal::rtos()]->send_signal(handle, signal);
    }

    uint32_t wait_signal()
    {
        init();

        if (task_osals[(uint32_t)osal::rtos()] == nullptr)
        {
            return 0;
        }

        return task_osals[(uint32_t)osal::rtos()]->wait_signal();
    }

    error::Error create_task(
        task::Func func, uint32_t id, uint16_t stack_depth, uint32_t priority, void **handle)
    {
        init();

        if (task_osals[(uint32_t)osal::rtos()] == nullptr)
        {
            return error::NoError;
        }

        return task_osals[(uint32_t)osal::rtos()]->create_task(
            func, id, stack_depth, priority, handle);
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

namespace task_osal_test
{


}

// End of File