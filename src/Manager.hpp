/**
 * @file Manager.hpp
 * @brief LoggerManager header file
 * @author bhhxx
 * @date 2025-05-20
*/
#pragma once
#include <unordered_map>
#include "AsynLogger.hpp"
namespace asynlog
{
/**
 * @brief LoggerManager class
 * @note
 * The LoggerManager class is a singleton that manages all loggers.
 * It provides methods to add, get, and check the existence of loggers.
 */
class LoggerManager {
private:
    std::mutex mtx_;                                            // mutex for thread safety
    AsynLogger::ptr default_logger_;                            // default logger
    std::unordered_map<std::string, AsynLogger::ptr> loggers_;  // map of loggers
public:

    /**
     * @brief Get the singleton instance of LoggerManager
     * @return LoggerManager& Reference to the singleton instance
     */
    static LoggerManager &GetInstance() {
        static LoggerManager instance;
        return instance;
    }

    /**
     * @brief Check if a logger exists
     * @param name Name of the logger
     * @return true if the logger exists, false otherwise
     */
    bool LoggerExists(const std::string &name) {
        std::lock_guard<std::mutex> lock(mtx_);
        return loggers_.find(name) != loggers_.end();
    }

    /**
     * @brief Add a logger to the manager
     * @param logger Logger to be added
     * @note The logger is moved into the manager.
     */
    void AddLogger(const AsynLogger::ptr &&logger) {
        if (LoggerExists(logger->Name()))
            return;
        std::lock_guard<std::mutex> lock(mtx_);
        loggers_[logger->Name()] = logger;
    }

    /**
     * @brief Get a logger by name
     * @param name Name of the logger
     * @return AsynLogger::ptr Pointer to the logger, or nullptr if not found
     */
    AsynLogger::ptr GetLogger(const std::string &name) {
        std::lock_guard<std::mutex> lock(mtx_);
        auto it = loggers_.find(name);
        if (it != loggers_.end()) {
            return it->second;
        }
        return nullptr;
    }

    /**
     * @brief Get the default logger
     * @return AsynLogger::ptr Pointer to the default logger
     */
    AsynLogger::ptr GetDefaultLogger() { return default_logger_; }
private:

    /**
     * @brief Private constructor for singleton pattern
     * @note Initializes the default logger
     */
    LoggerManager() {
        std::unique_ptr<LoggerBuilder> builder = std::make_unique<LoggerBuilder>();
        builder->BuildLoggerName("default_logger");
        default_logger_ = builder->Build();
        loggers_["default_logger"] = default_logger_;
    }
};
} // namespace asynlog