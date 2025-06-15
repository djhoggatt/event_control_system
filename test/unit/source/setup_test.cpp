/// @file setup_test.cpp
/// @author Denver Hoggatt
/// @brief Unit tests for the setup module.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "setup.hpp"
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


//--------------------------------------------------------------------------------------------------
//  Fakes/Mocks
//--------------------------------------------------------------------------------------------------

DEFINE_FFF_GLOBALS;

namespace task
{
    FAKE_VOID_FUNC(init);
}

namespace event
{
    FAKE_VOID_FUNC(init);
}

namespace io
{
    FAKE_VOID_FUNC(open);
}

namespace control
{
    FAKE_VOID_FUNC(open);
}

namespace hal
{
    FAKE_VOID_FUNC(init);
}

namespace settings
{
    FAKE_VOID_FUNC(init);
}

namespace power
{
    FAKE_VOID_FUNC(init);
}

//--------------------------------------------------------------------------------------------------
//  Private Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  Tests
//--------------------------------------------------------------------------------------------------

TEST(SetupTest, Init)
{
    setup::init();
    setup::init();
}

TEST(SetupTest, Open)
{
    setup::open();
    setup::open();
}

// End of File