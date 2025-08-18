#ifndef __ENGINE_ERRORS_HPP__
#define __ENGINE_ERRORS_HPP__

#include <array>
#include <iterator>

/** Error List. Please never compare an error against FAILED
 * Either do result != OK , or !result. This way, Error fail
 * values can be more detailed in the future.
 *
 * This is a generic error list, mainly for organizing a language of returning errors.
 *
 * Errors:
 * - Are added to the Error enum in core/error/error_list.h
 * - Have a description added to error_names in core/error/error_list.cpp
 * - Are bound with BIND_CORE_ENUM_CONSTANT() in core/core_constants.cpp
 * - Have a matching Android version in platform/android/java/lib/src/org/godotengine/godot/error/Error.kt
 */
enum class Errors {
    OK = 0,
    FAILED,                      // Generic fail error
    ERROR_UNAVAILABLE,           // What is requested is unsupported/unavailable
    ERROR_UNCONFIGURED,          // The object being used hasn't been properly set up yet
    ERROR_UNAUTHORIZED,          // Missing credentials for requested resource
    ERROR_PARAMETER_RANGE_ERROR, // Parameter given out of range (5)
    ERROR_OUT_OF_MEMORY,         // Out of memory
    ERROR_FILE_NOT_FOUND,
    ERROR_FILE_BAD_DRIVE,
    ERROR_FILE_BAD_PATH,
    ERROR_FILE_NO_PERMISSION,
    ERROR_FILE_ALREADY_IN_USE,
    ERROR_FILE_CANT_OPEN,
    ERROR_FILE_CANT_WRITE,
    ERROR_FILE_CANT_READ,
    ERROR_FILE_UNRECOGNIZED,
    ERROR_FILE_CORRUPT,
    ERROR_FILE_MISSING_DEPENDENCIES,
    ERROR_FILE_EOF,
    ERROR_CANT_OPEN, // Can't open a resource/socket/file
    ERROR_CANT_CREATE,
    ERROR_QUERY_FAILED,
    ERROR_ALREADY_IN_USE,
    ERROR_LOCKED, // resource is locked
    ERROR_TIMEOUT,
    ERROR_CANT_CONNECT,
    ERROR_CANT_RESOLVE,
    ERROR_CONNECTION_ERROR,
    ERROR_CANT_ACQUIRE_RESOURCE,
    ERROR_CANT_FORK,
    ERROR_INVALID_DATA,        // Data passed is invalid (30)
    ERROR_INVALID_PARAMETER,   // Parameter passed is invalid
    ERROR_ALREADY_EXISTS,      // When adding, item already exists
    ERROR_DOES_NOT_EXIST,      // When retrieving/erasing, if item does not exist
    ERROR_DATABASE_CANT_READ,  // database is full
    ERROR_DATABASE_CANT_WRITE, // database is full (35)
    ERROR_COMPILATION_FAILED,
    ERROR_METHOD_NOT_FOUND,
    ERROR_LINK_FAILED,
    ERROR_SCRIPT_FAILED,
    ERROR_CYCLIC_LINK,
    ERROR_INVALID_DECLARATION,
    ERROR_DUPLICATE_SYMBOL,
    ERROR_PARSE_ERROR,
    ERROR_BUSY,
    ERROR_SKIP,
    ERROR_HELP,            // user requested help!!
    ERROR_BUG,             // a bug in the software certainly happened, due to a double check failing or unexpected behavior.
    ERROR_PRINTER_ON_FIRE, // the parallel port printer is engulfed in flames
    ERROR_MAX,             // Not being returned, value represents the number of errors
};

extern std::array<const char *, (size_t)Errors::ERROR_MAX> errorNames;

#endif
