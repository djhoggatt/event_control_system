/// @file uart.hpp
/// @author Denver Hoggatt
/// @brief UART declarations
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#pragma once

#include "io.hpp"
#include "input.hpp"
#include "output.hpp"
#include "error.hpp"

#include <cstdint>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------


namespace uart
{
    //----------------------------------------------------------------------------------------------
    //  Public Constants
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Data Types
    //----------------------------------------------------------------------------------------------

    enum VirtualPort
    {
        UART_NONE,

        UART_CLI,

        NumPorts,
    };

    class UART
        : public input::Input
        , public output::Output
    {
        private:
            // -----------------------------------------------------------------
            //  Class Private Variables
            // -----------------------------------------------------------------


            // -----------------------------------------------------------------
            //  Class Private Functions
            // -----------------------------------------------------------------

            void set_output(void *data);

            void *get_by_id();

            void print(void *data, io::IODirection dir);

        public:
            // -----------------------------------------------------------------
            //  Class Public Variables
            // -----------------------------------------------------------------

            VirtualPort uart_port;

            // -----------------------------------------------------------------
            //  Class Public Functions
            // -----------------------------------------------------------------

            void init();

            // End of Class
    };

    //----------------------------------------------------------------------------------------------
    //  Classes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    void isr_read(char c);

    // End of Namespace
}

// End of File