/// @file timer_osal.cpp
/// @author Denver Hoggatt
/// @brief Timer OSAL definitions
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "timer_osal.hpp"
#include "macros.hpp"

#include <cstdint>

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

    bool running_list[(uint32_t)timer_osal::TimerID::NumIDs];

    timer_osal::TimerOSAL *timer_osals[(uint32_t)osal::ID::NumIDs];

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

#define DEF_RTOS(NAME) timer_osals[(uint32_t)osal::ID::NAME] = timer_osal::NAME##_get_funcs();
#include "rtos.def"
#undef DEF_RTOS

        inited = true;
    }

    // End of Anonymous Namespace
}

namespace timer_osal
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    void delay_ms(uint32_t delay_ms)
    {
        uint32_t start_time = curr_time_ms();
        while ((curr_time_ms() - start_time) < delay_ms)
        {
            // Do nothing
        }
    }

    bool is_running(TimerID id)
    {
        return running_list[(uint32_t)id];
    }

    uint32_t curr_time_ms()
    {
        init();

        if (timer_osals[(uint32_t)osal::rtos()] == nullptr)
        {
            return 0;
        }

        return timer_osals[(uint32_t)osal::rtos()]->curr_time_ms();
    }

    error::Error stop(TimerID id)
    {
        init();

        if (timer_osals[(uint32_t)osal::rtos()] == nullptr)
        {
            return error::NoError;
        }

        error::Error ret_val = timer_osals[(uint32_t)osal::rtos()]->stop(id);

        if (ret_val == error::NoError)
        {
            running_list[(uint32_t)id] = false;
        }

        return ret_val;
    }

    error::Error start(TimerID id)
    {
        init();

        if (timer_osals[(uint32_t)osal::rtos()] == nullptr)
        {
            return error::NoError;
        }

        error::Error ret_val = timer_osals[(uint32_t)osal::rtos()]->start(id);

        if (ret_val == error::NoError)
        {
            running_list[(uint32_t)id] = false;
        }

        return ret_val;
    }

    error::Error create(TimerID id, TimerCallbackFunc callback, uint32_t period_ms, bool continuous)
    {
        init();

        if (timer_osals[(uint32_t)osal::rtos()] == nullptr)
        {
            return error::NoError;
        }

        return timer_osals[(uint32_t)osal::rtos()]->create(id, callback, period_ms, continuous);
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

namespace timer_osal_test
{


}

// End of File