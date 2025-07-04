/// @file mutex_osal.cpp
/// @author Denver Hoggatt
/// @brief Mutex OSAL definitions
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "mutex_osal.hpp"
#include "osal.hpp"

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

    mutex_osal::MutexOSAL *mutex_osals[(uint32_t)osal::ID::NumIDs];

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

#define DEF_RTOS(NAME) mutex_osals[(uint32_t)osal::ID::NAME] = mutex_osal::NAME##_get_funcs();
#include "rtos.def"
#undef DEF_RTOS

        inited = true;
    }

    // End of Anonymous Namespace
}

namespace mutex_osal
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    error::Error take(uint32_t id)
    {
        init();

        if (mutex_osals[(uint32_t)osal::rtos()] == nullptr)
        {
            return error::NoError;
        }

        return mutex_osals[(uint32_t)osal::rtos()]->take(id);
    }

    error::Error give(uint32_t id)
    {
        init();

        if (mutex_osals[(uint32_t)osal::rtos()] == nullptr)
        {
            return error::NoError;
        }

        return mutex_osals[(uint32_t)osal::rtos()]->give(id);
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

namespace mutex_osal_test
{


}

// End of File