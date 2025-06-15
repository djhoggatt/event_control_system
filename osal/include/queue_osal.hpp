/// @file queue_osal.hpp
/// @author Denver Hoggatt
/// @brief Queue OSAL declarations
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
#include "task.hpp"

#include <cstdint>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------


namespace queue_osal
{
    //----------------------------------------------------------------------------------------------
    //  Public Constants
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Data Types
    //----------------------------------------------------------------------------------------------

    class QueueOSAL : public osal::OSAL
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

            void send(task::ID task_id, void *item);

            void *wait(task::ID task_id);

            error::Error create(task::ID task_id, uint32_t item_size, uint32_t queue_size);

            // End of Class
    };

    //----------------------------------------------------------------------------------------------
    //  Classes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    void send(task::ID task_id, void *item);

    void *wait(task::ID task_id);

    error::Error create(task::ID task_id, uint32_t item_size, uint32_t queue_size);

#define DEF_RTOS(NAME) QueueOSAL *NAME##_get_funcs();
#include "rtos.def"
#undef DEF_RTOS

    // End of Namespace
}

// End of File