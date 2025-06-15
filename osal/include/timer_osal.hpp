/// @file timer_osal.hpp
/// @author Denver Hoggatt
/// @brief Timer OSAL declarations
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#pragma once

#include "osal.hpp"
#include "error.hpp"

#include <cstdint>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------


namespace timer_osal
{
    //----------------------------------------------------------------------------------------------
    //  Public Constants
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Data Types
    //----------------------------------------------------------------------------------------------

    typedef void (*TimerCallbackFunc)(uint32_t curr_time_ms);

    enum class TimerID : uint32_t
    {
        Periodic,

        NumIDs,
    };

    //----------------------------------------------------------------------------------------------
    //  Classes
    //----------------------------------------------------------------------------------------------

    class TimerOSAL : public osal::OSAL
    {
        private:
            // -----------------------------------------------------------------
            //  Class Private Variables
            // -----------------------------------------------------------------


            // -----------------------------------------------------------------
            //  Class Private Functions
            // -----------------------------------------------------------------


        public:
            // -----------------------------------------------------------------
            //  Class Public Variables
            // -----------------------------------------------------------------


            // -----------------------------------------------------------------
            //  Class Public Functions
            // -----------------------------------------------------------------

            uint32_t curr_time_ms();

            error::Error stop(TimerID id);

            error::Error start(TimerID id);

            error::Error create(TimerID           id,
                                TimerCallbackFunc callback,
                                uint32_t          period_ms,
                                bool              continuous);

            // End of Class
    };

    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    bool is_running(TimerID id);

    void delay_ms(uint32_t delay_ms);

    uint32_t curr_time_ms();

    error::Error stop(TimerID id);

    error::Error start(TimerID id);

    error::Error create(TimerID           id,
                        TimerCallbackFunc callback,
                        uint32_t          period_ms,
                        bool              continuous);

#define DEF_RTOS(NAME) TimerOSAL *NAME##_get_funcs();
#include "rtos.def"
#undef DEF_RTOS

    // End of Namespace
}

// End of File