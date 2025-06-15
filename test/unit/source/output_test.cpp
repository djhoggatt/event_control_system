/// @file output_test.cpp
/// @author Denver Hoggatt
/// @brief Unit tests for the output module.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "output.hpp"
#include "macros.hpp"
#include "fff.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

//--------------------------------------------------------------------------------------------------
//  Private Constants
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  File Variables
//--------------------------------------------------------------------------------------------------

output::Output *output_list[2];
void           *outputted_data;
bool            has_outputted = false;

//--------------------------------------------------------------------------------------------------
//  Fakes/Mocks
//--------------------------------------------------------------------------------------------------

DEFINE_FFF_GLOBALS;

class TestOutput : public output::Output
{
    public:
        void set_output(void *data)
        {
            has_outputted  = true;
            outputted_data = data;
        }

        void print(void *data, io::IODirection dir)
        {
            UNUSED(data);
            UNUSED(dir);
        }

        void init()
        {
            // Nothing to do
        }
};

//--------------------------------------------------------------------------------------------------
//  Private Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  Tests
//--------------------------------------------------------------------------------------------------

TEST(OutputTest, GetOutputPreConds)
{
    TEST_ERROR(output::get_by_id(io::IOID::NumIDs));
}

TEST(OutputTest, GetOutput)
{
    io::IOID id1 = io::IOID::INPUT_1;
    io::IOID id2 = io::IOID::INPUT_2;

    TestOutput out1 = TestOutput();
    TestOutput out2 = TestOutput();
    out1.id         = id1;
    out2.id         = id2;

    output_list[0] = &out1;
    output_list[1] = &out2;

    output::init_output_list(output_list, 2);

    TestOutput *test_out1 = dynamic_cast<TestOutput *>(output::get_by_id(id1));
    TestOutput *test_out2 = dynamic_cast<TestOutput *>(output::get_by_id(id2));

    ASSERT_EQ(out1.id, test_out1->id);
    ASSERT_EQ(out2.id, test_out2->id);

    ASSERT_TRUE(&out1 == test_out1);
    ASSERT_TRUE(&out2 == test_out2);
}

TEST(InputTest, GetOutputByName)
{
    const char *test_name = "test_name";

    TestOutput out1 = TestOutput();
    out1.name       = test_name;

    output_list[0] = &out1;

    output::init_output_list(output_list, 1);

    TestOutput *test_out1 = dynamic_cast<TestOutput *>(output::get_by_name(test_name));

    ASSERT_EQ(out1.name, test_out1->name);

    ASSERT_TRUE(&out1 == test_out1);
}

TEST(OutputTest, SetOutput)
{
    TestOutput out1 = TestOutput();

    char str[32]     = "Test Str";
    out1.output_type = &typeid(const char *);
    out1.set<const char *>(str);

    char *test_str = (char *)outputted_data;

    ASSERT_TRUE(has_outputted);
    ASSERT_STREQ(str, test_str);
}

TEST(OutputTest, InitOutputInfo)
{
    TestOutput test_out = TestOutput();

    test_out.id        = io::IOID::UART_CONSOLE;
    test_out.direction = io::IODirection::output;
    test_out.init_output_info(&typeid(const char *), io::IOType::UART);

    ASSERT_EQ(test_out.output_type, &typeid(const char *));
    ASSERT_EQ(test_out.type, io::IOType::UART);
    ASSERT_EQ(test_out.direction, io::IODirection::output);

    TestOutput test_in = TestOutput();
    test_in.id         = io::IOID::UART_CONSOLE;
    test_in.direction  = io::IODirection::input;
    test_in.init_output_info(&typeid(const char *), io::IOType::UART);

    ASSERT_EQ(test_in.direction, io::IODirection::input_output);
}

TEST(OutputTest, CmdOutput)
{
    TestOutput test_out = TestOutput();
    test_out.cmd_output(0, nullptr);
}

// End of File