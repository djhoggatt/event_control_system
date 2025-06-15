/// @file output.cpp
/// @author Denver Hoggatt
/// @brief Output definitions.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "output.hpp"
#include "error.hpp"
#include "macros.hpp"

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

    output::Output **output_list;
    uint32_t         output_list_size;

    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------


    // End of Anonymous Namespace
}

namespace output
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    /// @brief Gets a pointer to the output associated with the given ID.
    /// @param id ID of the output.
    /// @return Output pointer.
    ///
    Output *get_by_id(io::IOID id)
    {
        REQUIRE(id < io::IOID::NumIDs, error::InvalidID);

        Output *ret_val = nullptr;

        for (uint32_t i = 0; i < output_list_size; i++)
        {
            if (output_list[i]->id == id)
            {
                ret_val = output_list[i];
                break;
            }
        }

        return ret_val;
    }

    /// @brief Gets a pointer to the output associated with the given name.
    /// @param name Name of the output.
    /// @return Output pointer.
    ///
    Output *get_by_name(const char *name)
    {
        REQUIRE(name != nullptr, error::InvalidPointer);

        Output *ret_val = nullptr;

        for (uint32_t i = 0; i < output_list_size; i++)
        {
            if (strcmp(output_list[i]->name, name) == 0)
            {
                ret_val = output_list[i];
                break;
            }
        }

        return ret_val;
    }

    /// @brief Initializes the output list.
    /// @param list List of outputs.
    /// @param size Size of the list.
    ///
    void init_output_list(output::Output **list, uint32_t size)
    {
        output_list      = list;
        output_list_size = size;

        for (uint32_t i = 0; i < output_list_size; i++)
        {
            output_list[i]->init();
        }
    }

    //----------------------------------------------------------------------------------------------
    //  Class Function Definitions
    //----------------------------------------------------------------------------------------------

    /// @brief Call this to initialize output values.
    /// @param type_id Type associated with the output.
    /// @param io_type Type of this output.
    ///
    void Output::init_output_info(const std::type_info *type_id, io::IOType io_type)
    {
        this->output_type = type_id;
        this->type        = io_type;

        if (this->direction == io::IODirection::input)
        {
            this->direction = io::IODirection::input_output;
        }
        else if (this->direction != io::IODirection::input_output)
        {
            this->direction = io::IODirection::output;
        }

        this->print_io = false;
    }

    /// @brief Commands will call this to print out the output. Outputs can override this to have
    /// the relevant commands support setting their values.
    /// @return String containing the value of the output.
    ///
    void Output::cmd_output(uint32_t argc, char **argv)
    {
        UNUSED(argc);
        UNUSED(argv);

        printf("Unrecognized Output Type\r\n");
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

namespace output_test
{


}

// End of File