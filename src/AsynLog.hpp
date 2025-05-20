/**
 * @file AsynLog.hpp
 * @brief AsynLog header file
 * @author bhhxx
 * @date 2025-05-20
*/
#pragma once
#include "Manager.hpp"
namespace asynlog
{
/**
 * @brief Get named logger
 * @param name Logger name
 * @return AsynLogger::ptr Pointer to the logger
*/
AsynLogger::ptr GetLogger(const std::string &name)
{
    return LoggerManager::GetInstance().GetLogger(name);
}

/**
 * @brief Get default logger
 * @return AsynLogger::ptr Pointer to the default logger
*/
AsynLogger::ptr GetDefaultLogger()
{
    return LoggerManager::GetInstance().GetDefaultLogger();
}

/**
 * @brief define log macros
 * @param fmt Format string
 * @param ... Arguments to format string
*/
#define Debug(fmt, ...) Debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Info(fmt, ...) Info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Warn(fmt, ...) Warn(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Error(fmt, ...) Error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Fatal(fmt, ...) Fatal(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

/**
 * @brief define log macros for default logger
 * @param fmt Format string
 * @param ... Arguments to format string
*/
#define DebugDefault(fmt, ...) asynlog::GetDefaultLogger()->Debug(fmt, ##__VA_ARGS__)
#define InfoDefault(fmt, ...) asynlog::GetDefaultLogger()->Info(fmt, ##__VA_ARGS__)
#define WarnDefault(fmt, ...) asynlog::GetDefaultLogger()->Warn(fmt, ##__VA_ARGS__)
#define ErrorDefault(fmt, ...) asynlog::GetDefaultLogger()->Error(fmt, ##__VA_ARGS__)
#define FatalDefault(fmt, ...) asynlog::GetDefaultLogger()->Fatal(fmt, ##__VA_ARGS__)
} // namespace asynlog