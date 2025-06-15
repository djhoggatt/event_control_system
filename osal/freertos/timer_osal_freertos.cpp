/// @file timer_osal_freertos.cpp
/// @author Denver Hoggatt
/// @brief Timer OSAL for freeRTOS
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "timer_osal.hpp"
#include "error.hpp"
#include "macros.hpp"
#include "FreeRTOS.h"
#include "timers.h"

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


    //----------------------------------------------------------------------------------------------
    //  Private Data Types
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Private Function Prototypes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  File Variables
    //----------------------------------------------------------------------------------------------

    timer_osal::TimerCallbackFunc callback_list[(uint32_t)timer_osal::TimerID::NumIDs];
    void                         *handle_list[(uint32_t)timer_osal::TimerID::NumIDs];

    timer_osal::TimerOSAL osal_instance;

    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------

    void timer_callback(xTimerHandle timer)
    {
        uint32_t id = (uintptr_t)pvTimerGetTimerID(timer);
        REQUIRE(id < (uint32_t)timer_osal::TimerID::NumIDs, error::InvalidID);

        timer_osal::TimerCallbackFunc callback = callback_list[id];
        if (callback != nullptr)
        {
            callback(timer_osal::curr_time_ms());
        }
    }

    // End of Anonymous Namespace
}

namespace timer_osal
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    TimerOSAL *freertos_get_funcs()
    {
        return &osal_instance;
    }

    //----------------------------------------------------------------------------------------------
    //  Class Function Definitions
    //----------------------------------------------------------------------------------------------

    uint32_t TimerOSAL::curr_time_ms()
    {
        return (xTaskGetTickCount() / configTICK_RATE_HZ) * 1000;
    }

    error::Error TimerOSAL::stop(TimerID id)
    {
        error::Error ret_val = error::NoError;

        bool valid_id = id < TimerID::NumIDs;
        if (!valid_id)
        {
            ret_val = error::InvalidID;
        }
        else if (valid_id)
        {
            xTimerHandle timer  = (xTimerHandle)handle_list[(uint32_t)id];
            bool         no_err = xTimerStop(timer, 0) == pdPASS;
            ret_val             = no_err ? error::NoError : error::StopFailed;
        }

        return ret_val;
    }

    error::Error TimerOSAL::start(TimerID id)
    {
        error::Error ret_val = error::NoError;

        bool valid_id = id < TimerID::NumIDs;
        if (!valid_id)
        {
            ret_val = error::InvalidID;
        }
        else if (valid_id)
        {
            xTimerHandle timer  = (xTimerHandle)handle_list[(uint32_t)id];
            bool         no_err = xTimerStart(timer, 0) == pdPASS;
            ret_val             = no_err ? error::NoError : error::StopFailed;
        }

        return ret_val;
    }

    error::Error TimerOSAL::create(TimerID           id,
                                   TimerCallbackFunc callback,
                                   uint32_t          period_ms,
                                   bool              continuous)
    {
        UNUSED(id);
        UNUSED(callback);
        UNUSED(period_ms);
        UNUSED(continuous);

        error::Error ret_val = error::NoError;

        char timer_name[]                  = "Timer0";
        timer_name[strlen(timer_name) - 1] = '0' + (char)id;

        xTimerHandle timer = xTimerCreate(timer_name,
                                          pdMS_TO_TICKS(period_ms),
                                          continuous ? pdTRUE : pdFALSE,
                                          (void *)((uintptr_t)id),
                                          timer_callback);

        if (id < TimerID::NumIDs)
        {
            handle_list[(uint32_t)id]   = (void *)timer;
            callback_list[(uint32_t)id] = callback;
        }
        else
        {
            ret_val = error::InvalidID;
        }

        ret_val = (timer == nullptr) ? error::InvalidTime : ret_val;

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