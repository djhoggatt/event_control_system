/// @file output.hpp
/// @author Denver Hoggatt
/// @brief Output declarations.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#pragma once


#include "io.hpp"
#include "error.hpp"

#include <cstdint>
#include <vector>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------


namespace output
{
    //----------------------------------------------------------------------------------------------
    //  Public Constants
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Data Types
    //----------------------------------------------------------------------------------------------

    class Output : virtual public io::IO
    {
        private:
            // -----------------------------------------------------------------
            //  Class Private Variables
            // -----------------------------------------------------------------


            // -----------------------------------------------------------------
            //  Class Private Functions
            // -----------------------------------------------------------------

            /// @brief Outrputs need to override this to provide output values.
            /// @return Current value of the output.
            ///
            virtual void set_output(void *data) = 0;

        public:
            // -----------------------------------------------------------------
            //  Class Public Variables
            // -----------------------------------------------------------------


            // -----------------------------------------------------------------
            //  Class Public Functions
            // -----------------------------------------------------------------

            virtual void cmd_output(uint32_t argc, char **argv);

            /// @brief Sets the output. This is implemented as a template to allow for type
            /// conversions, and generic type checking. The provided type must match the type
            /// associated with the output. If it does not, this will throw an error.
            /// @tparam T Type of the output.
            /// @param data Output value to set.
            ///
            template<typename T>
            void set(T data)
            {
                REQUIRE(this->output_type, error::InvalidPointer);
                REQUIRE(typeid(T) == *(this->output_type), error::InvalidType);
                if (this->print_io)
                {
                    this->print((void *)(uintptr_t)data, io::IODirection::output);
                }
                this->set_output((void *)(uintptr_t)data);
            }

            void init_output_info(const std::type_info *type_id, io::IOType io_type);

            // End of Class
    };

    //----------------------------------------------------------------------------------------------
    //  Classes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    void init_output_list(output::Output **list, uint32_t size);

    Output *get_by_id(io::IOID id);

    Output *get_by_name(const char *name);

    // End of Namespace
}

// End of File