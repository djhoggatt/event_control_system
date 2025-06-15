/// @file mutex_osal_freertos.cpp
/// @author Denver Hoggatt
/// @brief Mutex OSAL for freeRTOS
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "mutex.hpp"
#include "mutex_osal.hpp"
#include "FreeRTOS.h"
#include "semphr.h"

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

    mutex_osal::MutexOSAL osal_instance;

    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------

    void *get_handle(uint32_t id)
    {
        void *ret_val = nullptr;

        static SemaphoreHandle_t mutex_handles[static_cast<uint32_t>(mutex::ID::NumIDs)];
        if (id < static_cast<uint32_t>(mutex::ID::NumIDs))
        {
            mutex_handles[id]
                = (mutex_handles[id] == NULL) ? xSemaphoreCreateMutex() : mutex_handles[id];
            ret_val = (void *)(mutex_handles[id]);
        }

        return ret_val;
    }

    // End of Anonymous Namespace
}

namespace mutex_osal
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    MutexOSAL *freertos_get_funcs()
    {
        return &osal_instance;
    }

    //----------------------------------------------------------------------------------------------
    //  Class Function Definitions
    //----------------------------------------------------------------------------------------------

    error::Error MutexOSAL::take(uint32_t id)
    {
        error::Error ret_val = error::NoError;

        SemaphoreHandle_t handle = (SemaphoreHandle_t)get_handle(id);
        if (handle == nullptr)
        {
            ret_val = error::InvalidIndex;
        }
        else if (xSemaphoreTake(handle, portMAX_DELAY) != pdTRUE)
        {
            ret_val = error::TimedOut;
        }

        return ret_val;
    }

    error::Error MutexOSAL::give(uint32_t id)
    {
        error::Error ret_val = error::NoError;

        SemaphoreHandle_t handle = (SemaphoreHandle_t)get_handle(id);
        if (xSemaphoreGive(handle) != pdTRUE)
        {
            ret_val = error::NoMemory;
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