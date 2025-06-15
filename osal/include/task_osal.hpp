/// @file task_osal.hpp
/// @author Denver Hoggatt
/// @brief Task OSAL declarations
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


namespace task_osal
{
    //----------------------------------------------------------------------------------------------
    //  Public Constants
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Data Types
    //----------------------------------------------------------------------------------------------

    struct StackInfo
    {
            uint8_t *base;
            uint32_t size;
            uint32_t high_water;
    };

    //----------------------------------------------------------------------------------------------
    //  Classes
    //----------------------------------------------------------------------------------------------

    class TaskOSAL : public osal::OSAL
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

            StackInfo get_stack_info(task::ID id);

            void send_signal(void *handle, uint32_t signal);

            uint32_t wait_signal();

            error::Error create_task(task::Func func,
                                     uint32_t   id,
                                     uint16_t   stack_depth,
                                     uint32_t   priority,
                                     void     **handle);

            // End of Class
    };

    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    StackInfo get_stack_info(task::ID id);

    void send_signal(void *handle, uint32_t signal);

    uint32_t wait_signal();

    error::Error create_task(
        task::Func func, uint32_t id, uint16_t stack_depth, uint32_t priority, void **handle);

#define DEF_RTOS(NAME) TaskOSAL *NAME##_get_funcs();
#include "rtos.def"
#undef DEF_RTOS

    // End of Namespace
}

// End of File