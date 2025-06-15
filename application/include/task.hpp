/// @file task.hpp
/// @author Denver Hoggatt
/// @brief Task declarations
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#pragma once

#include "bits.hpp"

#include <cstdint>
#include <cstdio>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------


namespace task
{
    //----------------------------------------------------------------------------------------------
    //  Public Constants
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Data Types
    //----------------------------------------------------------------------------------------------

    enum class ID : uint32_t
    {

#define DEF(task_name, priority, depth) task_name,
#include "tasks.def"
#undef DEF

        NumIDs,
    };

    enum class Signal : uint32_t
    {
        // All tasks
        GlobalOpen      = bits::Bit_0, // Open-time synchronization
        GlobalRun       = bits::Bit_1, // Run-time sycnhronization
        GlobalEvent     = bits::Bit_2, // Event Signal
        GlobalTerminate = bits::Bit_3, // Terminate Task

        GlobalInvalid = bits::Bit_4, // Invalid signal

        // Open Task
        // The signals in the open task correspond to open synchronization signals, sent by each
        // task, to the open task, to tell the open task that the task has finished opening. There
        // should be no signals that correspond to the open task (other than the invalid signal).

        NumSigs,
    };

    typedef void (*Func)(void *argument);

    //----------------------------------------------------------------------------------------------
    //  Classes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    void print_maximum_stack_usage(bool dump);

    uint32_t num();

    ID get_id(Func func);

    void send_open_signal(const Func calling_func);

    void send_signal(ID task_id, Signal signal);

    void broadcast(Signal signal, const Func calling_func);

    void wait_strict(Signal signal);

    uint32_t wait_any();

    void init();

    // End of Namespace
}

// End of File