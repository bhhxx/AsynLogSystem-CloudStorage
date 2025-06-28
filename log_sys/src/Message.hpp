/** 
 * @file Message.hpp
 * @brief Message class for logging
 * @author bhhxx
 * @date 2025-05-18
*/

#pragma once
#include <string> // for string
#include <thread> // for thread
#include "Level.hpp" // for level
#include "Util.hpp" // for Now()
namespace asynlog
{
/**
 * @brief LogMessage class
 */
class LogMessage {
public:
    size_t line_;            // line num
    time_t ctime_;           // time
    std::string file_name_;  // file name
    std::string name_;       // log name
    std::string payload_;    // 
    std::thread::id tid_;    // thread id
    LogLevel::value level_;  // level of log
public:
    LogMessage() = default;
    /**
     * @brief LogMessage constructor
     * @param level The level of the log
     * @param filename The name of the file
     * @param line The line number
     * @param name The name of the log
     * @param payload The payload of the log
    */
    LogMessage(LogLevel::value level, std::string filename, size_t line, std::string name, std::string payload) :
        name_(name),
        file_name_(filename),
        line_(line),
        level_(level),
        payload_(payload),
        ctime_(Util::Date::Now()),
        tid_(std::this_thread::get_id()) {}
    /**
    * @brief formatter
    * @return The formatted log message
    * @example [23:56:27][140677926471488][INFO][log][test_logmessage.cpp:8]    This is a test log message
    * @note The function formats the log message.
    */
    std::string format() {
        std::stringstream ret;
        struct tm t;
        localtime_r(&ctime_, &t);
        char buf[128];
        strftime(buf, sizeof(buf), "%H:%M:%S", &t);
        std::string tmp1 = '[' + std::string(buf) + "][";
        std::string tmp2 = "][" + std::string(LogLevel::ToString(level_)) + "][" + name_ + "][" + file_name_ + ":" + std::to_string(line_) + "]\t" + payload_ + "\n";
        ret << tmp1 << tid_ << tmp2;
        return ret.str();
    }
};
} // namespace asynlog