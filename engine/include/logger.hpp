#ifndef __ENGINE_LOGGER_HPP__
#define __ENGINE_LOGGER_HPP__

#include "defines.hpp"

#include "spdlog/spdlog.h"

namespace Engine {
    #define LOG_WARNING_ENABLED 1
    #define LOG_INFO_ENABLED 1
    #define LOG_DEBUG_ENABLED 1
    #define LOG_TRACE_ENABLED 1

    /** Disable debug and trace logging for release builds. */
    #if RELEASE == 1
        #define LOG_DEBUG_ENABLED 0
        #define LOG_TRACE_ENABLED 0
    #endif

    enum class TypeErrors {
        FAILED_CREATE_GAME,
        FAILED_ASSIGNED_FUNCTION_GAME,
        FAILED_CREATE_APPLICATION,
        FAILED_APPLICATION_SHUTDOWN_GRACEFULLY
    };

    /** Logs a fatal-level message. */
    #define ENGINE_LOG_FATAL(...) spdlog::error(__VA_ARGS__);

    #ifndef ENGINE_LOG_ERROR
        /** Logs a error-level message. */
        #define ENGINE_LOG_ERROR(...) spdlog::error(__VA_ARGS__);
    #endif

    #if LOG_WARNING_ENABLED == 1
        /** Logs a warning-level message. */
        #define ENGINE_LOG_WARNING(...) spdlog::warn(__VA_ARGS__);
    #else
        /** Does nothing when LOG_WARN_ENABLED != 1 */
        #define ENGINE_LOG_WARNING(...)
    #endif

    #if LOG_INFO_ENABLED == 1
        /** Logs a info-level message. */
        #define ENGINE_LOG_INFO(...) spdlog::info(__VA_ARGS__);
    #else
        /** Does nothing when LOG_INFO_ENABLED != 1 */
        #define ENGINE_LOG_INFO(...)
    #endif

    #if LOG_DEBUG_ENABLED == 1
        /** Logs a debug-level message. */
        #define ENGINE_LOG_DEBUG(...) spdlog::debug(__VA_ARGS__);
    #else
        /** Does nothing when LOG_DEBUG_ENABLED != 1 */
        #define ENGINE_LOG_DEBUG(...)
    #endif

    #if LOG_TRACE_ENABLED == 1
        /** Logs a trace-level message. */
        #define ENGINE_LOG_TRACE(...) spdlog::trace(__VA_ARGS__);
    #else
        /** Does nothing when LOG_TRACE_ENABLED != 1 */
        #define ENGINE_LOG_TRACE(...)
    #endif
}

#endif
