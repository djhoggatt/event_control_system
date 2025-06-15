/// @file periodic.cpp
/// @author Denver Hoggatt
/// @brief Periodic definitions
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "periodic.hpp"
#include "timer_osal.hpp"
#include "error.hpp"
#include "mutex.hpp"

#include <cstdint>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------

namespace
{
    //----------------------------------------------------------------------------------------------
    //  Private Constants
    //----------------------------------------------------------------------------------------------

    constexpr uint32_t FIDELITY_MS = 1;

    //----------------------------------------------------------------------------------------------
    //  Private Data Types
    //----------------------------------------------------------------------------------------------

    struct Periodic
    {
            uint32_t               period_ms;
            periodic::CallbackFunc callback;

            bool     enabled;      // True if the periodic is currently running
            uint32_t last_call_ms; // Time of the last call
    };

    Periodic periodic_list[(uint32_t)periodic::ID::NumIDs];

    //----------------------------------------------------------------------------------------------
    //  Private Function Prototypes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  File Variables
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------

    /// @brief Loops through all callbacks, and calls them.
    /// @param curr_time_ms Current system time.
    ///
    void call_callbacks(uint32_t curr_time_ms)
    {
        for (uint32_t i = 0; i < (uint32_t)periodic::ID::NumIDs; i++)
        {
            bool elapsed
                = (curr_time_ms - periodic_list[i].last_call_ms) >= periodic_list[i].period_ms;

            if (periodic_list[i].enabled && elapsed)
            {
                INVAR(periodic_list[i].callback != nullptr, error::InvalidPointer);

                periodic_list[i].callback(curr_time_ms);
            }
        }
    }

    /// @brief Creates the timer.
    ///
    void create_timer()
    {
        static bool timer_created = false;
        if (timer_created)
        {
            return;
        }

        error::Error err
            = timer_osal::create(timer_osal::TimerID::Periodic, call_callbacks, FIDELITY_MS, true);

        INVAR(err == error::NoError, error::AppInitFailed);

        timer_created = true;
    }

    /// @brief Stops the timer if it should stop.
    ///
    void stop_timer()
    {
        create_timer();

        bool periodic_running = false;
        for (uint32_t i = 0; i < (uint32_t)periodic::ID::NumIDs; i++)
        {
            periodic_running |= periodic_list[i].enabled;
        }

        if (timer_osal::is_running(timer_osal::TimerID::Periodic))
        {
            return;
        }

        timer_osal::stop(timer_osal::TimerID::Periodic);
    }

    /// @brief Starts the timer.
    ///
    void start_timer()
    {
        create_timer();
        if (timer_osal::is_running(timer_osal::TimerID::Periodic))
        {
            return;
        }

        timer_osal::start(timer_osal::TimerID::Periodic);
    }

    // End of Anonymous Namespace
}

namespace periodic
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    /// @brief Stops the given timer.
    /// @param id ID of the timer.
    ///
    void stop(ID id)
    {
        REQUIRE(id < ID::NumIDs, error::InvalidID);

        mutex::take(mutex::ID::Periodic);

        periodic_list[(uint32_t)id].enabled = false;
        stop_timer();

        mutex::give(mutex::ID::Periodic);
    }

    /// @brief Starts the given timer.
    /// @param id ID of the timer.
    ///
    void start(ID id)
    {
        REQUIRE(id < ID::NumIDs, error::InvalidID);
        REQUIRE(periodic_list[(uint32_t)id].callback != nullptr, error::InvalidPointer);

        mutex::take(mutex::ID::Periodic);

        periodic_list[(uint32_t)id].last_call_ms = timer_osal::curr_time_ms();
        periodic_list[(uint32_t)id].enabled      = true;
        start_timer();

        mutex::give(mutex::ID::Periodic);
    }

    /// @brief Creates a timer.
    /// @param id ID of the timer to create.
    /// @param period_ms Period of the timer.
    /// @param func Callback that will be called when the periodic expires.
    ///
    void create(ID id, uint32_t period_ms, CallbackFunc func)
    {
        REQUIRE(id < ID::NumIDs, error::InvalidID);
        REQUIRE(period_ms > 0, error::InvalidTime);
        REQUIRE(func != nullptr, error::InvalidPointer);

        mutex::take(mutex::ID::Periodic);

        if (periodic_list[(uint32_t)id].enabled == true)
        {
            INVAR(periodic_list[(uint32_t)id].period_ms == period_ms, error::TooManyAttempts);
            INVAR(periodic_list[(uint32_t)id].callback == func, error::TooManyAttempts);
        }
        else
        {
            periodic_list[(uint32_t)id].period_ms = period_ms;
            periodic_list[(uint32_t)id].callback  = func;
        }

        mutex::give(mutex::ID::Periodic);
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

namespace periodic_test
{

    uint32_t get_period(periodic::ID id)
    {
        return periodic_list[(uint32_t)id].period_ms;
    }

    bool get_enabled(periodic::ID id)
    {
        return periodic_list[(uint32_t)id].enabled;
    }

}

// End of File