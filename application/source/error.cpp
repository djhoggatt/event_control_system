/// @file error.cpp
/// @author Denver Hoggatt
/// @brief Error handling definitions
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "error.hpp"
#include "power_hal.hpp"
#include "macros.hpp"

#include <cstdio>
#include <cinttypes>
#include <stdexcept>

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

    struct ErrorInfo
    {
            error::Error error;
            const char  *err_str;

            const char *file;
            const char *func;
            uint32_t    line;
    };

    //----------------------------------------------------------------------------------------------
    //  Private Function Prototypes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  File Variables
    //----------------------------------------------------------------------------------------------

#if defined(TESTING)
    error::HandlerType handler = error::HandlerType::Exception;
#elif defined(DEBUG_MODE)
    error::HandlerType handler = error::HandlerType::Halt;
#else
    error::HandlerType handler = error::HandlerType::Reboot;
#endif

    ErrorInfo error_defaults[] = {

        {
            .error   = error::Unknown,
            .err_str = "Unknown Error",
            .file    = nullptr,
            .func    = nullptr,
            .line    = 0,
        },

#define DEF(error_name, error_string, error_val) \
    {                                            \
        .error   = error::error_name,            \
        .err_str = error_string,                 \
        .file    = nullptr,                      \
        .func    = nullptr,                      \
        .line    = 0,                            \
    },
#include "errors.def"
#undef DEF

    };

    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------

    /// @brief Reset the processor
    ///
    void reset()
    {
#if !defined(TESTING)
        power_hal::reset();
#endif
    }

    /// @brief Returns the default information for the given error.
    ///
    ErrorInfo get_default(error::Error error)
    {
        ErrorInfo ret_val = error_defaults[0];

        const uint32_t num_errors = sizeof(error_defaults) / sizeof(error_defaults[0]);
        for (uint32_t i = 0; i < (num_errors - 1); i++)
        {
            if (error_defaults[i].error == error)
            {
                ret_val = error_defaults[i];
                break;
            }
        }

        return ret_val;
    }

    /// @brief Notifies that an error has occured.
    ///
    void notify(ErrorInfo err_info)
    {
        printf("%s detected at %s:%" PRIu32 "(%s)\r\n",
               err_info.err_str,
               err_info.file,
               err_info.line,
               err_info.func);
        fflush(stdout);
    }

    /// @brief handler for errors
    ///
    void handle_error(ErrorInfo err_info)
    {
        notify(err_info);

        switch (handler)
        {
            case error::HandlerType::Halt:
                while (true)
                {
                    // Halt processing

#if defined(TESTING)
                    break;
#endif
                }
                break;

            case error::HandlerType::Exception:
#if defined(TESTING)
                throw std::runtime_error(err_info.err_str);
#endif
                break;

            case error::HandlerType::Reboot:
            default:
                reset();
                break;
        }
    }

    /// @brief Helper function for asserting using the error handling
    /// specififed in this module.
    ///
    void do_assert(bool assert, error::Error err, const char *file, const char *func, uint32_t line)
    {
        if (!assert)
        {
            ErrorInfo err_info = get_default(err);
            err_info.file      = file;
            err_info.func      = func;
            err_info.line      = line;
            handle_error(err_info);
        }
    }

    // End of Anonymous Namespace
}

namespace error
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    /// @brief Used to verify pre-conditions in design-by-contract. Will assert if the given
    /// condition is not true.
    /// @param reqs Condition that will be tested.
    /// @param error Error to throw on a failure.
    /// @param file The file this is being called from.
    /// @param func The function this is being called from.
    /// @param line The line this is being called from.
    ///
    void require(bool reqs, Error error, const char *file, const char *func, uint32_t line)
    {
        do_assert(reqs, error, file, func, line);
    }

    /// @brief Used to verify invariants in design-by-contract. Will assert if the given
    /// condition is not true.
    /// @param reqs Condition that will be tested.
    /// @param error Error to throw on a failure.
    /// @param file The file this is being called from.
    /// @param func The function this is being called from.
    /// @param line The line this is being called from.
    ///
    void invariant(bool invars, Error error, const char *file, const char *func, uint32_t line)
    {
        do_assert(invars, error, file, func, line);
    }

    /// @brief Used to verify post-conditions in design-by-contract. Will assert if the given
    /// condition is not true.
    /// @param reqs Condition that will be tested.
    /// @param error Error to throw on a failure.
    /// @param file The file this is being called from.
    /// @param func The function this is being called from.
    /// @param line The line this is being called from.
    ///
    void ensure(bool ensures, Error error, const char *file, const char *func, uint32_t line)
    {
        do_assert(ensures, error, file, func, line);
    }

    /// @brief Gets the given parameter.
    /// @param setting Parameter to get.
    /// @param value Value that will be set to the parameter.
    /// @return No error on success.
    ///
    int32_t get_param(settings::ID setting, uintptr_t value)
    {
        Error ret_val = Error::NoError;

        switch (setting)
        {
            case settings::ID::ErrorHandleType:
                *((HandlerType *)value) = handler;
                break;

            default:
                ret_val = Error::UnknownType;
        }

        return (int32_t)ret_val;
    }

    /// @brief Sets the given parameter.
    /// @param setting Setting to set.
    /// @param value Value used to set the parameter.
    /// @param bootup True if the device is booting up, otherwise false.
    /// @return No error on success.
    ///
    int32_t set_param(settings::ID setting, uintptr_t value, bool bootup)
    {
        UNUSED(bootup);

        Error ret_val = Error::NoError;

        switch (setting)
        {
            case settings::ID::ErrorHandleType:
                handler = ((HandlerType)value);
                break;

            default:
                ret_val = Error::UnknownType;
        }

        return (int32_t)ret_val;
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

namespace error_test
{

}

// End of File