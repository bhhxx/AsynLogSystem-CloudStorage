/**
 * @file AsynWorker.hpp
 * @brief Asynchronous worker object use producer/consumer to seperate data gen and data flush.
 * @author bhhxx
 * @date 2025-05-20
*/
#pragma once
#include <functional> // for function
#include <mutex> // for mutex
#include <memory> // for shared_ptr
#include <atomic> // for atomic
#include <condition_variable> // for conditional_variable
#include <thread> // for thread
#include "AsynBuffer.hpp" // for Buffer

namespace asynlog
{
/**
 * @param ASYNC_SAFE: Asynchronous safe
 * @param ASYNC_UNSAFE: Asynchronous unsafe
*/
enum class AsynType { ASYNC_SAFE, ASYNC_UNSAFE };
using functor = std::function<void(Buffer&)>;

/**
 * @brief Asynchronous worker class
 * @note 
 * 1. The AsynWorker class is used to manage asynchronous logging.
 * 
 * 2. It uses a producer-consumer model to handle the logging.
 * 
 * 3. The class is thread-safe and can be used in a multi-threaded environment.
*/
class AsynWorker {
public:
    using ptr = std::shared_ptr<AsynWorker>;

    /**
     * @brief AsynWorker constructor
     * @param cb The callback function to be called when the buffer is full
     * @param _type The type of asynchronous logging (safe or unsafe)
     * @note 
     * 1. The constructor initializes the callback function and the type of asynchronous logging.
     * 
     * 2. It also starts a new thread for the worker.
    */
    AsynWorker(const functor& cb, AsynType _type = AsynType::ASYNC_SAFE):
        callback_(cb),
        asyn_type_(_type),
        stop_(false),
        thread_(std::thread(&AsynWorker::ThreadEntry, this))
        {}
    
    /**
     * @brief AsynWorker destructor
     * @note 
     * 1. The destructor stops the worker thread and cleans up the resources.
     * 
     * 2. It is important to call Stop() before destroying the object to ensure that the thread is stopped properly.
    */
    ~AsynWorker() { Stop(); }

    /**
     * @brief Push data into the producer buffer
     * @param data The data to be pushed into the buffer
     * @param len The length of the data to be pushed
    */
    void Push(const char* data, size_t len) { // producer
        std::unique_lock<std::mutex> lock(mtx_);
        if (asyn_type_ == AsynType::ASYNC_SAFE) {
            cond_producer_.wait(lock, [&](){ // using lambda function to pred
                return len <= buffer_producer_.WriteableSize();
            });
        }
        buffer_producer_.Push(data, len);
        cond_consumer_.notify_one();
    }

    /**
     * @brief Stop the worker thread
    */
    void Stop() {
        stop_ = true;
        cond_consumer_.notify_all();
        thread_.join();
    }

private:
    /**
     * @brief Consumer: Thread entry point
    */
    void ThreadEntry() { // consumer
        while (1) {
            { // use {} to limit the scope of the lock
                std::unique_lock<std::mutex> lock(mtx_);
                cond_consumer_.wait(lock, [&](){ // wait for producer produces data
                    return stop_ || !buffer_producer_.IsEmpty();
                });
                buffer_producer_.Swap(buffer_consumer_); // swap buffer
                if (asyn_type_ == AsynType::ASYNC_SAFE) {
                    cond_producer_.notify_one();
                }
            }
            if (buffer_consumer_.ReadableSize() > 0) {
                callback_(buffer_consumer_);
                buffer_consumer_.Reset();
            }            
            if (stop_ && buffer_producer_.IsEmpty()) return; // when stop and there is no data in producer buffer, return
        }
    }

private:
    AsynType asyn_type_;                    // the type of asyn
    std::atomic<bool> stop_;                // flag for stop
    std::mutex mtx_;                        // mutex
    asynlog::Buffer buffer_producer_;       // two buffer for producer and consumer
    asynlog::Buffer buffer_consumer_;
    std::condition_variable cond_producer_; // two cv for producer and consumer
    std::condition_variable cond_consumer_;
    std::thread thread_;                    // one thread for consumer
    functor callback_;                      // the functor to be excuited if there are something in consumer buffer
};

} // namespace asynlog

