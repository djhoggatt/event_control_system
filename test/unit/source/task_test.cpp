/// @file task_test.cpp
/// @author Denver Hoggatt
/// @brief Unit tests for the task module.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "task.hpp"
#include "task_test.hpp"
#include "error.hpp"
#include "macros.hpp"
#include "task_osal.hpp"
#include "FreeRTOSConfig.h"
#include "fff.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

//--------------------------------------------------------------------------------------------------
//  Private Constants
//--------------------------------------------------------------------------------------------------

constexpr uintptr_t UNIQUE_HANDLE = 22; // Arbitrarily chosen.
constexpr uint32_t  UNIQUE_SIGNAL = 32; // Arbitrarily chosen.

//--------------------------------------------------------------------------------------------------
//  File Variables
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  Fakes/Mocks
//--------------------------------------------------------------------------------------------------

DEFINE_FFF_GLOBALS;

namespace task_osal
{
    FAKE_VALUE_FUNC(error::Error, create_task, task::Func, uint32_t, uint16_t, uint32_t, void **);
    FAKE_VALUE_FUNC(uint32_t, wait_signal);
    FAKE_VOID_FUNC(send_signal, void *, uint32_t);
    FAKE_VALUE_FUNC(StackInfo, get_stack_info, task::ID);
}

namespace task_open
{
    FAKE_VOID_FUNC(task_func, void *);
}

namespace task_CANParse
{
    FAKE_VOID_FUNC(task_func, void *);
}

namespace task_control
{
    FAKE_VOID_FUNC(task_func, void *);
}

//--------------------------------------------------------------------------------------------------
//  Private Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  Tests
//--------------------------------------------------------------------------------------------------

TEST(TaskTest, InitTaskCreateFail)
{
    task_osal::create_task_fake.return_val = error::Error::TestFailed;
    TEST_ERROR(task::init());
}

TEST(TaskTest, Init)
{
    task_osal::create_task_fake.return_val = error::Error::NoError;
    task_osal::create_task_fake.call_count = 0;
    task::init();

    ASSERT_EQ(task_osal::create_task_fake.call_count, task::num());
}

TEST(TaskTest, DoubleInit)
{
    TEST_ERROR(task::init());
}

TEST(TaskTest, WaitAny)
{
    task_osal::wait_signal_fake.call_count = 0;
    task::wait_any();
    ASSERT_TRUE(task_osal::wait_signal_fake.call_count);
}

TEST(TaskTest, WaitStrictPreConds)
{
    TEST_ERROR(task::wait_strict(task::Signal::NumSigs));
}

TEST(TaskTest, WaitStrict)
{
    task_osal::wait_signal_fake.call_count = 0;
    task_osal::wait_signal_fake.return_val = (uint32_t)task::Signal::GlobalOpen;
    task::wait_strict(task::Signal::GlobalOpen);
    ASSERT_TRUE(task_osal::wait_signal_fake.call_count);
}

TEST(TaskTest, WaitStrictIgnore)
{
    RESET_FAKE(task_osal::wait_signal);

    uint32_t ret_sigs[]
        = { (uint32_t)task::Signal::GlobalEvent, (uint32_t)task::Signal::GlobalOpen };

    SET_RETURN_SEQ(task_osal::wait_signal, ret_sigs, 2);
    task::wait_strict(task::Signal::GlobalOpen);
    ASSERT_GE(task_osal::wait_signal_fake.call_count, 2);
}

TEST(TaskTest, BroadcastPreConds)
{
    TEST_ERROR(task::broadcast(task::Signal::NumSigs, task_open::task_func));
}

TEST(TaskTest, Broadcast)
{
    RESET_FAKE(task_osal::send_signal);
    task_test::set_handle_by_id(task::ID::open, (void *)UNIQUE_HANDLE);
    task::broadcast(task::Signal::GlobalRun, task_open::task_func);

    ASSERT_EQ(task_osal::send_signal_fake.call_count,
              task::num() - 1); // -1, sender task is not broadcast to

    for (uint32_t i = 0; i < task::num() - 1; i++)
    {
        ASSERT_NE(task_osal::send_signal_fake.arg0_history[i],
                  task_test::get_handle_from_id(task::ID::open));
    }
}

TEST(TaskTest, SendSignalPreConds)
{
    TEST_ERROR(task::send_signal(task::ID::NumIDs, task::Signal::GlobalOpen));
    TEST_ERROR(task::send_signal(task::ID::open, task::Signal::NumSigs));
}

TEST(TaskTest, SendSignal)
{
    task_osal::send_signal_fake.call_count = 0;
    task::send_signal(task::ID::open, task::Signal::GlobalOpen);

    ASSERT_EQ(task_osal::send_signal_fake.call_count, 1);
}

TEST(TaskTest, SendOpenSignalPreConds)
{
    TEST_ERROR(task::send_open_signal(nullptr));
}

TEST(TaskTest, SendOpenSignal)
{
    RESET_FAKE(task_osal::send_signal);
    task_test::set_open_sig_by_id(task::ID::open, UNIQUE_SIGNAL);
    task_test::set_handle_by_id(task::ID::open, (void *)UNIQUE_HANDLE);
    task::send_open_signal(task_open::task_func);

    ASSERT_EQ(task_osal::send_signal_fake.call_count, 1);
    ASSERT_EQ(task_osal::send_signal_fake.arg0_history[0],
              task_test::get_handle_from_id(task::ID::open));
    ASSERT_EQ(task_osal::send_signal_fake.arg1_history[0], UNIQUE_SIGNAL);
}

TEST(TaskTest, GetIDPreConds)
{
    TEST_ERROR(task::get_id(nullptr));
}

TEST(TaskTest, GetID)
{
    task::ID id = task::get_id(task_open::task_func);

    ASSERT_EQ(id, task::ID::open);
}

TEST(TaskTest, PrintStackUsage)
{
    uint8_t              stack[configMINIMAL_STACK_SIZE];
    task_osal::StackInfo stack_info           = { stack, sizeof(stack), 0 };
    task_osal::get_stack_info_fake.return_val = stack_info;

    task::print_maximum_stack_usage(true);

    ASSERT_TRUE(task_osal::get_stack_info_fake.call_count);
}

// End of File