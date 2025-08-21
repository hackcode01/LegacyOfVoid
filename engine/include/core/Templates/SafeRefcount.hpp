#ifndef __ENGINE_SAFE_REFCOUNT_HPP__
#define __ENGINE_SAFE_REFCOUNT_HPP__

#include "../Typedefs.hpp"

#ifdef DEV_ENABLED
#include "../Errors/ErrorMacros.hpp"
#endif

#include <atomic>

/** IWYU pragma: keep - Used in macro. */
#include <type_traits>

/**
 * Design goals for these classes:
 * - No automatic conversions or arithmetic operators,
 *   to keep explicit the use of atomics everywhere.
 * - Using acquire-release semantics, even to set the first value.
 *   The first value may be set relaxedly in many cases, but adding the distinction
 *   between relaxed and unrelaxed operation to the interface would make it needlessly
 *   flexible. There's negligible waste in having release semantics for the initial
 *   value and, as an important benefit, you can be sure the value is properly synchronized
 *   even with threads that are already running.
 */

/** These are used in very specific areas of the engine where it's critical that these guarantees are held. */
#define SAFE_NUMERIC_TYPE_PUN_GUARANTEES(m_type)                          \
    static_assert(sizeof(SafeNumeric<m_type>) == sizeof(m_type));         \
    static_assert(alignof(SafeNumeric<m_type>) == alignof(m_type));       \
    static_assert(std::is_trivially_destructible_v<std::atomic<m_type>>); \

#define SAFE_FLAG_TYPE_PUN_GUARANTEES                 \
    static_assert(sizeof(SafeFlag) == sizeof(bool));  \
    static_assert(alignof(SafeFlag) == alignof(bool));

template <typename T>
class SafeNumeric {

};

#endif
