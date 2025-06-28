/**
 * @file LogFlush.hpp
 * @brief Provides the LogFlush class and its derived classes for logging.
 * @author bhhxx
 * @date 2025-05-20
*/
#pragma once
#include <iostream> // for cout
#include <memory> // for shared_ptr
#include <string> // for string 
#include <unistd.h> // for fsync
#include "Util.hpp" // for Util::File, Util::Date
extern asynlog::Util::JsonData* conf_data; // singleton instance of JsonData
namespace asynlog 
{

/**
 * @class LogFlush
 * @brief Abstract base class for log flushing.
 */
class LogFlush {
public:
    using ptr = std::shared_ptr<LogFlush>;
    virtual ~LogFlush() {}
    virtual void Flush(const char*data, size_t len) = 0;
};

/**
 * @class StdOutFlush
 * @brief Derived class for flushing logs to standard output.
 */
class StdOutFlush : public LogFlush {
public:
    using ptr = std::shared_ptr<LogFlush>;

    /**
     * @brief Flushes the log message to standard output.
     * @param data The log message data.
     * @param len The length of the log message data.
     */
    void Flush(const char*data, size_t len) override {
        std::cout.write(data, len);
    }
};

/**
 * @class FileFlush
 * @brief Derived class for flushing logs to a file.
 */
class FileFlush : public LogFlush {
private:
    std::string filename_;    // log file name
    FILE* fs_ = NULL;         // file pointer
public:
    using ptr = std::shared_ptr<FileFlush>;

    /**
     * @brief Constructs a new FileFlush object.
     * @param filename The name of the log file.
     * @note This function creates the directory for the log file if it does not exist.
     */
    FileFlush(const std::string &filename) : filename_(filename)
    {
        Util::File::CreateDirectory(Util::File::Path(filename));
        fs_ = fopen(filename.c_str(), "ab"); // "ab" mode for appending
        if(fs_ == NULL){
            std::cout <<__FILE__<<__LINE__<< "open log file failed" << std::endl;
            perror(NULL);
        }
    }

    /**
     * @brief Flushes the log message to the log file.
     * @param data The log message data.
     * @param len The length of the log message data.
     */
    void Flush(const char *data, size_t len) override{
        fwrite(data, 1, len, fs_);
        if(ferror(fs_)){
            std::cout <<__FILE__<<__LINE__<< "write log file failed" << std::endl;
            perror(NULL);
        }
        if(conf_data->flush_log == 1) {
            if(fflush(fs_) == EOF){
                std::cout << __FILE__ << __LINE__ << "fflush file failed" << std::endl;
                perror(NULL);
            }
        } else if (conf_data->flush_log == 2) {
            fflush(fs_);
            fsync(fileno(fs_));
        }
    }
};


/**
 * @class RollFileFlush
 * @brief Derived class for flushing logs to a rolling file.
 */
class RollFileFlush : public LogFlush {
private:
    size_t cnt_ = 1;           // rolling file count
    size_t cur_size_ = 0;      // current file size
    size_t max_size_;          // maximum file size
    std::string basename_;     // base name of the log file
    FILE* fs_ = NULL;          // file pointer
public:
    using ptr = std::shared_ptr<RollFileFlush>;

    /**
     * @brief Constructs a new RollFileFlush object.
     * @param filename The base name of the log file.
     * @param max_size The maximum size of the log file.
     * @note This function creates the directory for the log file if it does not exist.
     */
    RollFileFlush(const std::string &filename, size_t max_size) : max_size_(max_size), basename_(filename) {
        Util::File::CreateDirectory(Util::File::Path(basename_));
    }

    /**
     * @brief Flushes the log message to the rolling log file.
     * @param data The log message data.
     * @param len The length of the log message data.
     */
    void Flush(const char*data, size_t len) {
        InitLogFile();
        fwrite(data, 1, len, fs_);
        if(ferror(fs_)){
            std::cout <<__FILE__<<__LINE__<< "write log file failed" << std::endl;
            perror(NULL);
        }
        cur_size_ += len;
        if(conf_data->flush_log == 1) {
            if(fflush(fs_) == EOF){
                std::cout << __FILE__ << __LINE__ << "fflush file failed" << std::endl;
                perror(NULL);
            }
        } else if (conf_data->flush_log == 2) {
            fflush(fs_);
            fsync(fileno(fs_));
        }
    }
private:

    /**
     * @brief Initializes the log file.
     * @note This function creates a new log file if the current file size exceeds the maximum size.
     */
    void InitLogFile() {
        if (fs_ == NULL || cur_size_ >= max_size_) {
            if (fs_ != NULL) {
                fclose(fs_);
                fs_ = NULL;
            }
            std::string filename = CreateFilename();
            fs_ = fopen(filename.c_str(), "ab");
            if (fs_ == NULL) {
                std::cout << __FILE__ << __LINE__ << std::endl;
                perror(NULL);
            }
            cur_size_ = 0;
        }
    }

    /**
     * @brief Creates a new log file name based on the current date and time.
     * @return The new log file name.
     */
    std::string CreateFilename() {
        time_t time_ = Util::Date::Now();
        struct tm t;
        localtime_r(&time_, &t);
        std::string filename = basename_;
        filename += std::to_string(t.tm_year + 1900);
        filename += std::to_string(t.tm_mon + 1);
        filename += std::to_string(t.tm_mday);
        filename += std::to_string(t.tm_hour + 1);
        filename += std::to_string(t.tm_min + 1);
        filename += std::to_string(t.tm_sec + 1) + '-' +
                    std::to_string(cnt_++) + ".log";
        return filename;
    }
};

class LogFlushFactory {
public:
    using ptr = std::shared_ptr<LogFlushFactory>;

    /**
     * @brief Creates a new log flush object.
     * @param FlushType The type of the log flush object to create.
     * @param args The arguments to pass to the constructor of the log flush object.
     * @return A shared pointer to the new log flush object.
     */
    template<typename FlushType, typename... Args>
    static std::shared_ptr<LogFlush> CreateLog(Args &&...args)
    {
        return std::make_shared<FlushType>(std::forward<Args>(args)...);
    }
};
} // asynlog