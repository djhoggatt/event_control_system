/// @file input.cpp
/// @author Denver Hoggatt
/// @brief Input definitions
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "input.hpp"
#include "error.hpp"

#include <cstdint>
#include <cstring>
#include <cstdio>

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

    input::Input **input_list;
    uint32_t       input_list_size;

    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------


    // End of Anonymous Namespace
}

namespace input
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    /// @brief Gets a pointer to the input associated with the given ID.
    /// @param id ID of the input.
    /// @return Input pointer.
    ///
    Input *get_by_id(io::IOID id)
    {
        REQUIRE(id < io::IOID::NumIDs, error::InvalidID);

        Input *ret_val = nullptr;

        for (uint32_t i = 0; i < input_list_size; i++)
        {
            if (input_list[i]->id == id)
            {
                ret_val = input_list[i];
                break;
            }
        }

        return ret_val;
    }

    /// @brief Gets a pointer to the input associated with the given name.
    /// @param name Name of the input.
    /// @return Input pointer.
    ///
    Input *get_by_name(const char *name)
    {
        REQUIRE(name != nullptr, error::InvalidPointer);

        Input *ret_val = nullptr;

        for (uint32_t i = 0; i < input_list_size; i++)
        {
            if (strcmp(input_list[i]->name, name) == 0)
            {
                ret_val = input_list[i];
                break;
            }
        }

        return ret_val;
    }

    /// @brief Initializes the input list.
    /// @param list List of inputs.
    /// @param size Size of the list.
    ///
    void init_input_list(input::Input **list, uint32_t size)
    {
        input_list      = list;
        input_list_size = size;

        for (uint32_t i = 0; i < input_list_size; i++)
        {
            input_list[i]->init();
        }
    }

    //----------------------------------------------------------------------------------------------
    //  Class Function Definitions
    //----------------------------------------------------------------------------------------------

    /// @brief Call this to initialize input values.
    /// @param type_id Type associated with the input.
    /// @param io_type Type of this input.
    ///
    void Input::init_input_info(const std::type_info *type_id, io::IOType io_type)
    {
        this->input_type = type_id;
        this->type       = io_type;

        if (this->direction == io::IODirection::output)
        {
            this->direction = io::IODirection::input_output;
        }
        else if (this->direction != io::IODirection::input_output)
        {
            this->direction = io::IODirection::input;
        }

        this->print_io = false;
    }

    /// @brief Commands will call this to print out the input. Inputs can override this to have the
    /// relevant commands support getting their values.
    /// @return String containing the value of the input.
    ///
    char *Input::cmd_input()
    {
        static char str[] = "Unrecognized Input Type\r\n";

        return str;
    }

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

namespace input_test
{


}

// End of File