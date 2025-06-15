/// @file error.hpp
/// @author Denver Hoggatt
/// @brief Error handling declarations
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#pragma once

#include "settings.hpp"

#include <cstdint>
#include <climits>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------

#define REQUIRE(req, err) (error::require(req, err, __FILE__, __func__, __LINE__))
#define INVAR(inv, err)   (error::invariant(inv, err, __FILE__, __func__, __LINE__))
#define ENSURE(ens, err)  (error::ensure(ens, err, __FILE__, __func__, __LINE__))

namespace error
{
    //----------------------------------------------------------------------------------------------
    //  Public Constants
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Data Types
    //----------------------------------------------------------------------------------------------

    /// @brief Handler type for errors.
    ///
    enum class HandlerType : uint32_t
    {
        Exception,
        Halt,
        Reboot,
    };

    /// @brief An error.
    ///
    /// Note: This is a traditional enum both so that this can be easily converted to signed
    /// integers, and to reduce the number of namespace accesses that are needed to obtain an error
    /// (these will be used a lot).
    ///
    enum Error
    {

#define DEF(error_name, error_string, error_val) error_name = error_val,
#include "errors.def"
#undef DEF

        Unknown = INT_MIN

    };

    //----------------------------------------------------------------------------------------------
    //  Classes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    void require(bool reqs, Error error, const char *file, const char *func, uint32_t line);

    void invariant(bool invars, Error error, const char *file, const char *func, uint32_t line);

    void ensure(bool ensures, Error err_on_fail, const char *file, const char *func, uint32_t line);

    int32_t get_param(settings::ID setting, uintptr_t value);

    int32_t set_param(settings::ID setting, uintptr_t value, bool boot);

    // End of Namespace
}

// End of File