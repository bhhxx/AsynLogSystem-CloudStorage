/**
 * @file ThreadPool.hpp
 * @brief Provides a thread pool implementation for asynchronous task execution.
 * @author bhhxx
 * @date 2025-05-18
 * @note This file is part of the asynlog logging library.
 */
#pragma once
#include <vector> // for vector
#include <thread> // for thread
#include <queue> // for queue
#include <functional> // for function
#include <mutex> // for mutex
#include <condition_variable> // for condition_variable
#include <future> // for future
#include <stdexcept> // for runtime_error 
#include <memory> // for make_shared

/** 
 * @brief ThreadPool class
 * @note This class provides a thread pool implementation for asynchronous task execution.
*/
class ThreadPool
{
private:
    std::vector<std::thread> workers;             // threads
    std::queue<std::function<void()>> tasks;      // tasks queue
    std::mutex queue_mutex;                       // queue mutex
    std::condition_variable condition;            // condition_variable for syn
    bool stop;
public:
    /**
     * @brief Construct a new Thread Poll object
     * @param thread_size number of threads
     */
    ThreadPool(size_t thread_size) : stop(false) {
        for (size_t i = 0; i < thread_size; i++) {
            workers.emplace_back( // use lambda function to create thread
                [this] {
                    while (true) {
                        std::function<void()> task; // create task object
                        {
                            std::unique_lock<std::mutex> lock(this->queue_mutex);
                            this->condition.wait(lock, [this]{return this->stop || !this->tasks.empty();}); // wait until there is a task or stop
                            if (this->stop && this->tasks.empty()) return;
                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                        }
                        task();
                    }
                }
            );
        }
    }

    /**
     * @brief Enqueue a task
     * @param f function to be executed
     * @param args arguments to be passed to the function
     * @return future object
     */
    template <class F, class... Args> // template function
    auto enqueue(F &&f, Args &&...args)->std::future<std::invoke_result_t<F, Args...>> {
        using return_type = std::invoke_result_t<F, Args...>;
        auto task = std::make_shared<
            std::packaged_task<return_type()>
        >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (stop) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }
            tasks.emplace([task]{(*task)();});
        }
        condition.notify_one();
        return res;
    }

    /**
     * @brief Stop the thread pool
     */
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker : workers) {
            worker.join();
        }
    }
};