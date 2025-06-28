/**
 * @file AsynBuffer.hpp
 * @brief Buffer class
 * @author bhhxx
 * @date 2025-05-12
 */
#pragma once
#include <vector> // for vector
#include <string> // for string
#include <cassert> // for assert
#include "Util.hpp" // for JsonData
extern asynlog::Util::JsonData* conf_data;

namespace asynlog {
/**
 * @brief Buffer class
 * @note 
 * The Buffer class is used to manage a buffer for asynchronous logging.
 */
class Buffer { 
protected:
    size_t write_pos_;
    size_t read_pos_;
    std::vector<char> buffer_;
public:
    /**
     * @brief Buffer constructor
     * @note
     * 1. The constructor initializes the buffer size to the configured size.
     * 
     * 2. The write position and read position are initialized to 0.
     */
    Buffer() {
        write_pos_ = 0;
        read_pos_ = 0;
        buffer_.resize(conf_data->buffer_size);
    }
    
    /**
     * @brief Push data into the buffer
     * @param data The data to be pushed into the buffer
     * @param len The length of the data to be pushed
     * @note ToBeEnough() is called to ensure that there is ToBeEnough space in the buffer.
     */
    void Push(const char* data, size_t len) {
        ToBeEnough(len);
        std::copy(data, data + len, &buffer_[write_pos_]);
        write_pos_ += len;
    }

    /**
     * @brief Read data from the buffer
     * @param len The length of the data to be read
     * @return A pointer to the beginning of the data to be read
     * @note The function asserts that the length is less than or equal to the readable size.
     */
    char *ReadBegin(size_t len) {
        assert(len <= ReadableSize());
        return &buffer_[read_pos_];
    }

    /**
     * @brief Get the beginning pointer
     * @return A pointer to the beginning of the data to be read
     */
    const char *Begin() { return &buffer_[read_pos_]; }
    
    /**
     * @brief Get the writable size of the buffer
     * @return The size of the buffer that can be written to
     */
    size_t WriteableSize() {
        return buffer_.size() - write_pos_;
    }

    /**
     * @brief Get the readable size of the buffer
     * @return The size of the buffer that can be read from
     */
    size_t ReadableSize() {
        return write_pos_ - read_pos_;
    }

    /**
     * @brief Swap the contents of two buffers
     * @param buf The buffer to swap with
     * @note The function swaps the contents of the current buffer with the given buffer.
     */
    void Swap(Buffer &buf) {
        buffer_.swap(buf.buffer_);
        std::swap(read_pos_, buf.read_pos_);
        std::swap(write_pos_, buf.write_pos_);
    }

    /**
     * @brief check if the buffer is empty
     * @return true if the buffer is empty, false otherwise
     */
    bool IsEmpty() {
        return write_pos_ == read_pos_;
    }

    /**
     * @brief reset the buffer
     * @note The function resets the write and read positions to 0.
     */
    void Reset() {
        write_pos_ = 0;
        read_pos_ = 0;
    }

    /**
     * @brief move the read position
     * @param len The length to move the read position
     */
    void MoveReadPos(size_t len) {
        assert(len <= ReadableSize());
        read_pos_ += len;
    }

    /**
     * @brief move the write position
     * @param len The length to move the write position
     */
    void MoveWritePos(size_t len) {
        assert(len <= WriteableSize());
        write_pos_ += len;
    }
protected:
    /**
     * @brief Ensure that there is enough space in the buffer
     * @param len The length of the data to be pushed
     * @note The function resizes the buffer if the length is greater than the writable size.
     * It uses a linear growth strategy if the buffer size is greater than the threshold.
     * Otherwise, it triples the size of the buffer.
     * The threshold is defined in the configuration data.
     */
    void ToBeEnough(size_t len) {
        size_t buffer_size = buffer_.size();
        if (len >= WriteableSize()) {
            if (buffer_.size() < conf_data->threshold) { // triple the size
                buffer_.resize(2 * buffer_.size() + buffer_size);
            } else {
                buffer_.resize(conf_data->linear_growth + buffer_size); // using linear growth
            }
        }
    }
};

} // namespace asynlog