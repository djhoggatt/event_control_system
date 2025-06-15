/// @file periodic.hpp
/// @author Denver Hoggatt
/// @brief Periodic declarations
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#pragma once

#include <cstdint>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------


namespace periodic
{
    //----------------------------------------------------------------------------------------------
    //  Public Constants
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Data Types
    //----------------------------------------------------------------------------------------------

    typedef void (*CallbackFunc)(uint32_t curr_time_ms);

    enum class ID : uint32_t
    {
        Test,
        ADCConversion,

        NumIDs,
    };

    //----------------------------------------------------------------------------------------------
    //  Classes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    void stop(ID id);

    void start(ID id);

    void create(ID id, uint32_t period_ms, CallbackFunc func);

    // End of Namespace
}

// End of File