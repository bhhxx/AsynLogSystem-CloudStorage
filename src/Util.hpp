/**
 * @file Util.hpp
 * @brief Provides utility functions for the asynlog logging library.
 * @author bhhxx
 * @date 2025-05-10
 * @note config path now is `../src/config.json` you need to change it to your own path.
 * * This file is part of the asynlog logging library.
 */
#pragma once
#include <ctime> // for time
#include <sys/stat.h> // for stat
#include <fstream> // for istream
#include <string>
#include <iostream>
#include <jsoncpp/json/json.h> // for json
namespace asynlog {
namespace Util {
/**
 * @class Date
 * @brief Provides Now() function to get the current system time. 
 */
class Date {
public:
    /**
     * @brief Gets the current system time.
     * @return time_t The current calendar time expressed in seconds since 
     * the Epoch (`00:00:00 UTC, January 1, 1970`).
     */
    static time_t Now() { return time(nullptr); }
};

/**
 * @class File
 * @brief Provides utility functions for file operations.
 */
class File {
public:
    /**
     * @brief Checks if a file exists.
     * @param filename The name of the file to check.
     * @return true if the file exists, false otherwise.
     */
    static bool Exists(const std::string& filename) {
        struct stat st;
        return stat(filename.c_str(), &st) == 0;
    }

    /** 
     * @brief Gets the directory name of a file.
     * @param filename The name of the file.
     * @return The directory name of the file. (include the last `'/'`)
    */
    static std::string Path(const std::string& filename) {
        if (filename.empty()) {
            return "";
        }
        auto pos = filename.find_last_of("/\\");
        if (pos == std::string::npos) {
            return "";
        } else {
            return filename.substr(0, pos + 1);
        }
    }
    
    /**
     * @brief Creates a directory if it does not exist.
     * @param pathname The name of the directory to create.
     */
    static void CreateDirectory(const std::string& pathname) {
        if (pathname.empty()) {
            // do nothing but send a warning
            std::cerr << "CreateDirectory: Path name is empty" << std::endl;
            return;
        }
        if (!Exists(pathname)) {
            size_t pos, index = 0;
            size_t size = pathname.size();
            while (index < size) {
                pos = pathname.find("/", index); // find the first '/' after index
                if (pos == std::string::npos) {
                    mkdir(pathname.c_str(), 0755);
                    return;
                }
                if (pos == index) {
                    index = pos + 1;
                    continue;
                }
                std::string sub_path = pathname.substr(0, pos);
                if (sub_path == "." || sub_path == "..") {
                    index = pos + 1;
                    continue;
                }
                if (Exists(sub_path)) {
                    index = pos + 1;
                    continue;
                }
                mkdir(sub_path.c_str(), 0755);
                index = pos + 1;
            }
        }
    }

    /**
     * @brief Gets the size of a file.
     * @param filename The name of the file.
     * @return The size of the file in bytes. If the file does not exist, returns -1.
     */
    static int64_t FileSize(std::string filename) {
        struct stat st;
        auto ret = stat(filename.c_str(), &st);
        if (ret == -1) {
            std::cerr << "Get file size failed" << std::endl;
            return -1;
        }
        return st.st_size;
    }

    /**
     * @brief Reads the content of a file.
     * @param content The content of the file.
     * @param filename The name of the file.
     * @return true if the file is read successfully, false otherwise.
     */
    static bool GetContent(std::string *content, std::string filename) {
        std::ifstream ifs;
        ifs.open(filename.c_str(), std::ios::binary);
        if (!ifs.is_open()) {
            std::cerr << "Open file failed" << std::endl;
            return false;
        }
        ifs.seekg(0, std::ios::beg); // move the cursor to the beginning of the file
        size_t len = FileSize(filename);
        content->resize(len);
        ifs.read(&(*content)[0], len);
        if (!ifs.good()) {
            std::cout << __FILE__ << __LINE__ << "-" << "read file content error" << std::endl;
            return false;
        }
        ifs.close();
        return true;
    }
};

/**
 * @class JsonUtil
 * @brief Provides utility functions for JSON operations.
 */
class JsonUtil {
public:
    /**
     * @brief Serializes a Json::Value object to a JSON string.
     * @param val The Json::Value object to serialize.
     * @param str The JSON string.
     * @return true if the serialization is successful, false otherwise.
     * @note This function uses Json::StreamWriterBuilder to serialize the Json::Value object to a JSON string.
     */
    static bool Serialize(const Json::Value &val, std::string *str) {
        Json::StreamWriterBuilder swb;
        std::unique_ptr<Json::StreamWriter> usw(swb.newStreamWriter());
        std::stringstream ss;
        if (usw->write(val, &ss) != 0) {
            std::cout << __FILE__ << __LINE__ << "-" << "serialize json error" << std::endl;
            return false;
        }
        *str = ss.str();
        return true;
    }

    /**
     * @brief UnSerialize a JSON string to a Json::Value object.
     * @param str The JSON string.
     * @param val The Json::Value object to unserialize.
     * @return true if the deserialization is successful, false otherwise.
     * @note This function uses Json::CharReaderBuilder to unserialize the JSON string to a Json::Value object.
     */
    static bool UnSerialize(const std::string &str, Json::Value *val) {
        Json::CharReaderBuilder crb;
        std::unique_ptr<Json::CharReader> ucr(crb.newCharReader());
        std::string err;
        if (!ucr->parse(str.c_str(), str.c_str() + str.size(), val, &err)) {
            std::cout << __FILE__ << __LINE__ << "-" << "unserialize json error" << std::endl;
            return false;
        }
        return true;
    }
};

class JsonData {
public:

    /**
     * @brief Gets the JsonData object.
     * @return The JsonData object.
     * @note This function uses a static variable to store the JsonData object.
     * @note This function is thread-safe.
     * @note This function is lazy-initialized.
     */
    static JsonData* GetJsonData() {
        static JsonData* json_data = new JsonData();
        return json_data;
    }
private:
    /**
     * @brief Constructs a new JsonData object.
     * @note This function reads the content of the config.json file and stores it in the JsonData object.
     */
    JsonData() {
        std::string content;
        asynlog::Util::File file;
        if (file.GetContent(&content, "../src/config.json") == false) {
            std::cout << __FILE__ << __LINE__ << "-" << "read config.json error" << std::endl;
            return;
        }
        Json::Value root;
        asynlog::Util::JsonUtil::UnSerialize(content, &root);
        buffer_size = root["buffer_size"].asInt64();
        threshold = root["threshold"].asInt64();
        linear_growth = root["linear_growth"].asInt64();
        flush_log = root["flush_log"].asInt64();
        backup_addr = root["backup_addr"].asString();
        backup_port = root["backup_port"].asInt();
        thread_count = root["thread_count"].asInt();
    }
public:
    int64_t buffer_size;    // buffer size in bytes
    size_t threshold;       // threshold for buffer size in bytes
    size_t linear_growth;   // linear growth for buffer size in bytes
    size_t flush_log;       // 
    std::string backup_addr;// backup address
    uint16_t backup_port;   // backup port
    size_t thread_count;    // thread pool size
};
} // namespace Util   
} // namespace aynlog
