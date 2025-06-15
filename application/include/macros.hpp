/// @file macros.hpp
/// @author Denver Hoggatt
/// @brief Various macros and compile-time functions.
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#pragma once

#include <cstdint>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------

#define UNUSED(x) ((void)x)

#define TEST_ERROR(x)                       \
    do                                      \
    {                                       \
        bool error_thrown = false;          \
        try                                 \
        {                                   \
            x;                              \
        }                                   \
        catch (const std::runtime_error &e) \
        {                                   \
            error_thrown = true;            \
        }                                   \
        ASSERT_TRUE(error_thrown);          \
    } while (false);

#define REENTRY_GUARD(x)                   \
    do                                     \
    {                                      \
        static bool reentry_guard = false; \
        if (reentry_guard)                 \
        {                                  \
            return x;                      \
        }                                  \
        reentry_guard = true;              \
    } while (false);

#define REENTRY_GUARD_CLASS(x)       \
    do                               \
    {                                \
        this->reentry_guard = false; \
        if (reentry_guard)           \
        {                            \
            return x;                \
        }                            \
        this->reentry_guard = true;  \
    } while (false);

/// @brief Compile-time digit counter using recursion.
/// @param num Number to count the digits of.
/// @return Number of digits.
///
constexpr uint32_t count_digits(uint32_t num)
{
    return (num < 10) ? 1 : 1 + count_digits(num / 10);
}

/// @brief Compile-time string length counter using recursion.
/// @param str String to count.
/// @return Number of characters in the string.
///
constexpr uint32_t const_str_len(const char *str)
{
    return (*str == '\0') ? 0 : 1 + const_str_len(str + 1);
}

/// @brief Clamps the given value between the low and high values (inclusive).
/// @param value Value to clamp.
/// @param low Lower bound.
/// @param high Upper bound.
/// @return Clamped value.
///
template<typename T>
constexpr T clamp(T value, T low, T high)
{
    return (value < low) ? low : ((value > high) ? high : value);
}

// End of File