/// @file task_osal_freertos.cpp
/// @author Denver Hoggatt
/// @brief Task OSAL for freeRTOS
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "task_osal.hpp"
#include "isr_hal.hpp"
#include "macros.hpp"
#include "FreeRTOS.h"
#include "task.h"

#include <cstdint>
#include <cstring>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------

namespace
{
    //----------------------------------------------------------------------------------------------
    //  Private Constants
    //----------------------------------------------------------------------------------------------

    constexpr uint8_t WATERMARK = 0xAA;

    //----------------------------------------------------------------------------------------------
    //  Private Data Types
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Private Function Prototypes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  File Variables
    //----------------------------------------------------------------------------------------------

    void    *handles[(uint32_t)task::ID::NumIDs];
    uint32_t stack_sizes[(uint32_t)task::ID::NumIDs];

    StaticTask_t stack_bufs[(uint32_t)task::ID::NumIDs];

#define DEF(TASK_NAME, PRIORITY, DEPTH) StackType_t stack_##TASK_NAME[DEPTH];
#include "tasks.def"
#undef DEF

    StackType_t *stacks[(uint32_t)task::ID::NumIDs] = {
#define DEF(TASK_NAME, PRIORITY, DEPTH) stack_##TASK_NAME,
#include "tasks.def"
#undef DEF
    };

    task_osal::TaskOSAL osal_instance;

    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------


    // End of Anonymous Namespace
}

namespace task_osal
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    TaskOSAL *freertos_get_funcs()
    {
        return &osal_instance;
    }

    //----------------------------------------------------------------------------------------------
    //  Class Function Definitions
    //----------------------------------------------------------------------------------------------

    StackInfo TaskOSAL::get_stack_info(task::ID id)
    {
        StackInfo ret_val;

        uint32_t high_water = 0; // FreeRTOS stack grows down, not up
        for (uint32_t i = 0; i < stack_sizes[(uint32_t)id]; i++)
        {
            if (((uint8_t *)(stacks[(uint32_t)id]))[i] != WATERMARK)
            {
                high_water = i;
                break;
            }
        }

        ret_val.base       = (uint8_t *)stacks[(uint32_t)id];
        ret_val.size       = stack_sizes[(uint32_t)id];
        ret_val.high_water = ret_val.size - high_water; // Adjust for grow down

        return ret_val;
    }

    void TaskOSAL::send_signal(void *handle, uint32_t signal)
    {
        if (isr_hal::is_in_interrupt())
        {
            BaseType_t yield = pdFALSE; // ISR context may need to yield, see
                                        // freertos documentation for more details.
            xTaskNotifyFromISR((TaskHandle_t)handle, signal, eSetBits, &yield);
            portYIELD_FROM_ISR();
        }
        else
        {
            xTaskNotify((TaskHandle_t)handle, signal, eSetBits);
        }
    }

    uint32_t TaskOSAL::wait_signal()
    {
        uint32_t notify_val = 0;

        xTaskNotifyWait(pdFALSE, ULONG_MAX, &notify_val, portMAX_DELAY);

        return notify_val;
    }

    error::Error TaskOSAL::create_task(
        task::Func func, uint32_t id, uint16_t stack_depth, uint32_t priority, void **handle)
    {
        error::Error ret_val = error::NoError;

        char name[]            = "Task0";
        name[strlen(name) - 1] = '0' + (id % 10);

        memset(stacks[id], WATERMARK, stack_depth * sizeof(StackType_t));
        *handle = xTaskCreateStatic(
            func, name, stack_depth, NULL, (UBaseType_t)priority, stacks[id], &stack_bufs[id]);

        handles[id]     = *handle;
        stack_sizes[id] = stack_depth * sizeof(StackType_t);

        if (*handle == nullptr) // Error creating task
        {
            vTaskDelete(*((TaskHandle_t *)handle));
            ret_val = error::DeviceInitFailed;
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