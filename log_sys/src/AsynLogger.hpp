/**
 * @file AsynLogger.hpp
 * @brief AsynLogger class: basic logging; LoggerBuilder: using builder model to create log object.
 * @author bhhxx
 * @date 2025-05-20
*/
#pragma once
#include <string> // for string
#include <memory> // for make_shared, shared_ptr
#include <vector> // for vector
#include <cstdarg> // for va_start
#include <assert.h> // for assert

#include "AsynWorker.hpp" // for AsynWorker
#include "LogFlush.hpp" // for LogFlush, StdOutFlush, FileFlush, RollFileFlush
#include "Level.hpp" // for LogLevel
#include "Message.hpp" // for LogMessage
#include "ThreadPool.hpp" // for ThreadPool
#include "backup/ClientBackup.hpp"

extern ThreadPool *tp;

namespace asynlog
{
/**
 * @brief AsynLogger class
 * @details This class is used to log messages asynchronously.
*/
class AsynLogger {
private:
    std::string logger_name_;              // logger's name
    AsynType asyntype_;                    // type of async
    std::shared_ptr<AsynWorker> worker_;   // produer and consumer
    std::vector<LogFlush::ptr> flushes_;   // vector for different Flush
public:
    using ptr = std::shared_ptr<AsynLogger>;

    /**
     * @brief AsynLogger constructor
     * @param logger_name name of the logger
     * @param asyntype type of async
     * @param flushes vector of flushes
     * @details This constructor initializes the logger with the given name, async type and flushes.
    */
    AsynLogger(const std::string logger_name, AsynType asyntype, std::vector<LogFlush::ptr> flushes) :
        logger_name_(logger_name),
        asyntype_(asyntype),
        flushes_(flushes) {                                   // functor         who to call      the first param  
            worker_ = std::make_shared<AsynWorker>(std::bind(&AsynLogger::RealFlush, this, std::placeholders::_1), asyntype);
        }
    /**
     * @brief AsynLogger destructor
    */
    ~AsynLogger() {}

    /**
     * @brief Info Error Warn Fatal Debug log
     * @param file filename where log generates
     * @param line line number where log generates
     * @param format the log content
     * @param ... additional arguments
    */
    void Info(const std::string &file, size_t line, const std::string format, ...) {
        va_list va;
        va_start(va, format);
        char *ret;
        int r = vasprintf(&ret, format.c_str(), va);
        if (r == -1) {
            perror("vasprintf failed!!!: ");
        }
        va_end(va);
        std::string single_log_data;
        serialize(LogLevel::value::INFO, file, line, ret, single_log_data);
        worker_->Push(single_log_data.c_str(), single_log_data.size()); // push formatted log to buffer
        free(ret);
        ret = nullptr;
    }

    void Error(const std::string &file, size_t line, const std::string format, ...) {
        va_list va;
        va_start(va, format);
        char *ret;
        int r = vasprintf(&ret, format.c_str(), va);
        if (r == -1) {
            perror("vasprintf failed!!!: ");
        }
        va_end(va);
        std::string single_log_data;
        serialize(LogLevel::value::ERROR, file, line, ret, single_log_data);
        worker_->Push(single_log_data.c_str(), single_log_data.size()); // push formatted log to buffer
        free(ret);
        ret = nullptr;
    }

    void Warn(const std::string &file, size_t line, const std::string format, ...) {
        va_list va;
        va_start(va, format);
        char *ret;
        int r = vasprintf(&ret, format.c_str(), va);
        if (r == -1) {
            perror("vasprintf failed!!!: ");
        }
        va_end(va);
        std::string single_log_data;
        serialize(LogLevel::value::WARN, file, line, ret, single_log_data);
        worker_->Push(single_log_data.c_str(), single_log_data.size()); // push formatted log to buffer
        free(ret);
        ret = nullptr;
    }

    void Fatal(const std::string &file, size_t line, const std::string format, ...) {
        va_list va;
        va_start(va, format);
        char *ret;
        int r = vasprintf(&ret, format.c_str(), va);
        if (r == -1) {
            perror("vasprintf failed!!!: ");
        }
        va_end(va);
        std::string single_log_data;
        serialize(LogLevel::value::FATAL, file, line, ret, single_log_data);
        worker_->Push(single_log_data.c_str(), single_log_data.size()); // push formatted log to buffer
        free(ret);
        ret = nullptr;
    }

    void Debug(const std::string &file, size_t line, const std::string format, ...) {
        va_list va; // variable param
        va_start(va, format); // using format to locate variable list
        char *ret;
        int r = vasprintf(&ret, format.c_str(), va); // using va to replace the symbol like %d, and ret store the final str
        if (r == -1) {
            perror("vasprintf failed!!!: ");
        }
        va_end(va); // pair with va_start for robustness
        std::string single_log_data;
        serialize(LogLevel::value::DEBUG, file, line, ret, single_log_data);
        worker_->Push(single_log_data.c_str(), single_log_data.size()); // push formatted log to buffer
        free(ret);
        ret = nullptr;
    }

    /**
     * @brief Get the logger name
     * @return logger name
    */
    std::string Name() { return logger_name_; }

protected:
    
    /**
     * @brief Serialize the log message and push it to the worker
     * @param level log level
     * @param file name of the file which log
     * @param line line number of the log
     * @param ret the log message
    */
    void serialize(LogLevel::value level, const std::string &file, size_t line, char *ret, std::string &data) {
        LogMessage msg(level, file, line, logger_name_, ret);
        data = msg.format();
        if (level == LogLevel::value::FATAL || level == LogLevel::value::ERROR) {
            try {
                auto ret = tp->enqueue(start_log_backup, data);
                ret.get();
            }
            catch (const std::runtime_error &e) {
                std::cout << __FILE__ << __LINE__ << "thread pool closed" << std::endl;
            }
        }
    }

    /**
     * @brief the call back function for the worker to flush the log message
     * @param buffer buffer for the log message
    */
    void RealFlush (Buffer &buffer) {
        if (flushes_.empty()) {
            return;
        }
        for (auto &e : flushes_) {
            if (e) {
                e->Flush(buffer.Begin(), buffer.ReadableSize());
            }
        }
    }
};

/**
 * @brief LoggerBuilder class
 * @details This class is used to build the logger.
*/
class LoggerBuilder {
public:
    using ptr = std::shared_ptr<LoggerBuilder>;

    /**
     * @brief Build the logger name
     * @param name name of the logger
    */
    void BuildLoggerName(const std::string &name) { logger_name_ = name; }

    /**
     * @brief Build the logger type
     * @param type type of the logger
    */
    void BuildLoggerType(AsynType type) { asyn_type_ = type; }

    /**
     * @brief Build the logger flush
     * @param flush flush type
     * @param args additional arguments
    */
    template <typename FlushType, typename... Args>
    void BuildLoggerFlush(Args &&...args) {
        flushes_.emplace_back(
            LogFlushFactory::CreateLog<FlushType>(std::forward<Args>(args)...)
        );
    }

    /**
     * @brief Build the logger
     * @return AsynLogger pointer
    */
    AsynLogger::ptr Build() {
        assert(logger_name_.empty() == false);
        if (flushes_.empty()) {
            flushes_.emplace_back(std::make_shared<StdOutFlush>());
        }
        return std::make_shared<AsynLogger>(
            logger_name_, asyn_type_, flushes_
        );
    }
protected:
    // these initialized variable can be used by default_log
    std::string logger_name_ = "async_logger";      // default logger name
    std::vector<asynlog::LogFlush::ptr> flushes_;   // vector for different Flush
    AsynType asyn_type_ = AsynType::ASYNC_SAFE;     // default async type
};
} // namespace asynlog