/// @file queue_osal_freertos.cpp
/// @author Denver Hoggatt
/// @brief Queue OSAL for freeRTOS
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "queue_osal.hpp"
#include "isr_hal.hpp"
#include "FreeRTOS.h"
#include "queue.h"
#include "portmacro.h"

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

    queue_osal::QueueOSAL osal_instance;

    static QueueHandle_t queues[static_cast<uint32_t>(task::ID::NumIDs)];

    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------

    void *get_handle_from_id(task::ID task_id)
    {
        void *ret_val = nullptr;

        if (task_id < task::ID::NumIDs)
        {
            uint32_t id = static_cast<uint32_t>(task_id);
            ret_val     = (void *)queues[id];
        }

        return ret_val;
    }

    // End of Anonymous Namespace
}

namespace queue_osal
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    QueueOSAL *freertos_get_funcs()
    {
        return &osal_instance;
    }

    //----------------------------------------------------------------------------------------------
    //  Class Function Definitions
    //----------------------------------------------------------------------------------------------

    void QueueOSAL::send(task::ID task_id, void *item)
    {
        QueueHandle_t handle = (QueueHandle_t)get_handle_from_id(task_id);
        uint32_t      id     = static_cast<uint32_t>(task_id);
        if ((handle != nullptr) && isr_hal::is_in_interrupt())
        {
            BaseType_t yield = pdFALSE; // ISR context may need to yield, see
                                        // freertos documentation for more details.
            xQueueSendToBackFromISR(queues[id], item, &yield);
            portYIELD_FROM_ISR();
        }
        else if (handle != nullptr)
        {
            xQueueSendToBack(queues[id], item, portMAX_DELAY);
        }
    }

    void *QueueOSAL::wait(task::ID task_id)
    {
        void *ret_val = nullptr;

        QueueHandle_t handle = (QueueHandle_t)get_handle_from_id(task_id);
        xQueueReceive(handle, (void *)&ret_val, portMAX_DELAY);

        return ret_val;
    }

    error::Error QueueOSAL::create(task::ID task_id, uint32_t item_size, uint32_t queue_size)
    {
        error::Error ret_val = error::NoError;

        QueueHandle_t handle = xQueueCreate(queue_size, item_size);
        if (handle == NULL)
        {
            ret_val = error::NoMemory;
        }
        else
        {
            uint32_t id = static_cast<uint32_t>(task_id);
            queues[id]  = handle;
        }

        return ret_val;
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

namespace mutex_osal_test
{


}

// End of File