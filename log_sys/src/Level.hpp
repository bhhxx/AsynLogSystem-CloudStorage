/**
 * @file Level.hpp
 * @brief LogLevel class
 * @author bhhxx
 * @date 2025-05-10
 */
#pragma once // include guard
#include <string>

namespace asynlog {
    class LogLevel {
    public:
        // enum class to avoid name conflicts with other libraries
        enum class value { DEBUG, INFO, WARN, ERROR, FATAL};
        static const char* ToString(value level) {
            switch (level) {
                case value::DEBUG: return "DEBUG"; // use value to show the enum value
                case value::INFO: return "INFO ";
                case value::WARN: return "WARN ";
                case value::ERROR: return "ERROR";
                case value::FATAL: return "FATAL";
                default: return "UNKNOWN";
            }
        }
    };
}
