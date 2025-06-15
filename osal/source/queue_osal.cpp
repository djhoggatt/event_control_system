/// @file queue_osal.cpp
/// @author Denver Hoggatt
/// @brief Queue OSAL deifnitions
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "queue_osal.hpp"
#include "isr_hal.hpp"
#include "error.hpp"

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

    queue_osal::QueueOSAL *queue_osals[(uint32_t)osal::ID::NumIDs];

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

#define DEF_RTOS(NAME) queue_osals[(uint32_t)osal::ID::NAME] = queue_osal::NAME##_get_funcs();
#include "rtos.def"
#undef DEF_RTOS

        inited = true;
    }

    // End of Anonymous Namespace
}

namespace queue_osal
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    void send(task::ID task_id, void *item)
    {
        init();

        if (queue_osals[(uint32_t)osal::rtos()] == nullptr)
        {
            return;
        }

        queue_osals[(uint32_t)osal::rtos()]->send(task_id, item);
    }

    void *wait(task::ID task_id)
    {
        init();

        if (queue_osals[(uint32_t)osal::rtos()] == nullptr)
        {
            return nullptr;
        }

        return queue_osals[(uint32_t)osal::rtos()]->wait(task_id);
    }

    error::Error create(task::ID task_id, uint32_t item_size, uint32_t queue_size)
    {
        init();

        if (queue_osals[(uint32_t)osal::rtos()] == nullptr)
        {
            return error::NoError;
        }

        return queue_osals[(uint32_t)osal::rtos()]->create(task_id, item_size, queue_size);
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

namespace queue_osal_test
{


}

// End of File