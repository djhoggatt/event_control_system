/// @file template.hpp
/// @author Denver Hoggatt
/// @brief Brief description of the file
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


namespace input
{
    //----------------------------------------------------------------------------------------------
    //  Public Constants
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Data Types
    //----------------------------------------------------------------------------------------------

    class Input : virtual public io::IO
    {
        private:
            // -----------------------------------------------------------------
            //  Class Private Variables
            // -----------------------------------------------------------------


            // -----------------------------------------------------------------
            //  Class Private Functions
            // -----------------------------------------------------------------

            /// @brief Inputs need to override this to provide input values.
            /// @return Current value of the input.
            ///
            virtual void *get_by_id() = 0;

        public:
            // -----------------------------------------------------------------
            //  Class Public Variables
            // -----------------------------------------------------------------


            // -----------------------------------------------------------------
            //  Class Public Functions
            // -----------------------------------------------------------------

            virtual char *cmd_input();

            /// @brief Gets the input. This is implemented as a template to allow for type
            /// conversions, and generic type checking. The provided type must match the type
            /// associated with the input. If it does not, this will throw an error.
            /// @tparam T Type of the input.
            /// @return Current value of the input.
            ///
            template<typename T>
            T get()
            {
                REQUIRE(this->input_type, error::InvalidPointer);
                REQUIRE(typeid(T) == *(this->input_type), error::InvalidType);
                T ret_val = (T)(uintptr_t)get_by_id();
                if (this->print_io)
                {
                    this->print((void *)(uintptr_t)ret_val, io::IODirection::input);
                }
                return ret_val;
            }

            void init_input_info(const std::type_info *type_id, io::IOType io_type);

            // End of Class
    };

    //----------------------------------------------------------------------------------------------
    //  Classes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    void init_input_list(input::Input **list, uint32_t size);

    Input *get_by_id(io::IOID id);

    Input *get_by_name(const char *name);

    // End of Namespace
}

// End of File