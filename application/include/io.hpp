/// @file io.hpp
/// @author Denver Hoggatt
/// @brief IO definitions.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#pragma once

#include <cstdint>
#include <typeinfo>

#include "io_id.hpp"

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------


namespace io
{
    //----------------------------------------------------------------------------------------------
    //  Public Constants
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Data Types
    //----------------------------------------------------------------------------------------------

    enum class IOType
    {
        GPIO,
        ADC,
        PWM,
        UART,
        SPI,
    };

    enum class IODirection
    {
        none,
        input,
        output,
        input_output,
    };

    //----------------------------------------------------------------------------------------------
    //  Classes
    //----------------------------------------------------------------------------------------------

    class IO
    {
        private:
            // -----------------------------------------------------------------
            //  Class Private Variables
            // -----------------------------------------------------------------


            // -----------------------------------------------------------------
            //  Class Private Functions
            // -----------------------------------------------------------------


        public:
            // -----------------------------------------------------------------
            //  Class Public Variables
            // -----------------------------------------------------------------

            IOType      type;
            IOID        id;
            IODirection direction = IODirection::none;
            const char *name;
            bool        print_io;
            IO         *parent;
            bool        reentry_guard = false;

            // This is a bit tricky, but in short, we want a way to verify that the type when
            // setting/getting from an IO is the proper type associated with this IO. For example,
            // we don't want to set UART output with a PWM duty cycle, and want to check that it's
            // being set with a string. Thus, we can use the typeid function to check this. typeid
            // returns a type_info class, which we can reference during initialization as I believe
            // this class exists for the runtime of the program
            //
            const std::type_info *input_type;
            const std::type_info *output_type;

            // -----------------------------------------------------------------
            //  Class Public Functions
            // -----------------------------------------------------------------

            virtual void print(void *data, IODirection dir) = 0;

            virtual void init() = 0;

            // End of Class
    };

    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    void print(const char *io, const char *name, IOID id, char *data, IODirection dir);

    IO *get_by_name(const char *name);

    IO *get_by_id(io::IOID id);

    IOType get_type(IOID id);

    void open();

    // End of Namespace
}

// End of File