/// @file io.cpp
/// @author Denver Hoggatt
/// @brief IO definitions.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "io.hpp"
#include "input.hpp"
#include "output.hpp"

#include <vector>
#include <cstdio>
#include <cinttypes>

#include "io_list.cpp" // Do not include anywhere else

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


    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------


    // End of Anonymous Namespace
}

namespace io
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    /// @brief Generic function for printing IO when data is sent/received.
    /// @param io IO Type.
    /// @param name IO Name.
    /// @param id IO ID.
    /// @param data Data sent/received.
    /// @param dir IO Direction.
    ///
    void print(const char *io, const char *name, IOID id, char *data, IODirection dir)
    {
        if (dir == IODirection::input)
        {
            printf("Received Data. IO: %s, Name: %s, ID: %" PRIu32 ", Data: %s\r\n",
                   io,
                   name,
                   (uint32_t)id,
                   data);
        }
        else if (dir == IODirection::output)
        {
            printf("Sent Data. IO: %s, Name: %s, ID: %" PRIu32 ", Data: %s\r\n",
                   io,
                   name,
                   (uint32_t)id,
                   data);
        }
    }

    /// @brief Get's a pointer to the IO using the given name.
    /// @param name Name of the IO.
    /// @return IO pointer.
    ///
    IO *get_by_name(const char *name)
    {
        IO *ret_val = (IO *)output::get_by_name(name);
        if (ret_val == nullptr)
        {
            ret_val = (IO *)input::get_by_name(name);
        }

        return ret_val;
    }

    /// @brief Get's a pointer to the IO using the given id.
    /// @param id ID of the IO.
    /// @return IO pointer.
    ///
    IO *get_by_id(io::IOID id)
    {
        IO *ret_val = (IO *)output::get_by_id(id);
        if (ret_val == nullptr)
        {
            ret_val = (IO *)input::get_by_id(id);
        }

        return ret_val;
    }

    /// @brief Returns the type of the IO.
    /// @param id ID of the IO.
    /// @return Type of the IO.
    ///
    IOType get_type(IOID id)
    {
        REQUIRE(id < IOID::NumIDs, error::InvalidID);

        return io_type_list[(uint32_t)id];
    }

    /// @brief Opens the IO module.
    ///
    void open()
    {
        init_io();

        uint32_t input_size  = sizeof(io_input_list) / sizeof(io_input_list[0]);
        uint32_t output_size = sizeof(io_output_list) / sizeof(io_output_list[0]);

        input::init_input_list(io_input_list, input_size);
        output::init_output_list(io_output_list, output_size);
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

namespace io_test
{


}

// End of File