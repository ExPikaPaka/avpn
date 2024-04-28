#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <future>

class ThreadPool {
public:
    // Constructor that creates a specified number of worker threads
    explicit ThreadPool(std::size_t numThreads);

    // Destructor that waits for all threads and tasks to complete
    ~ThreadPool();

    // Removing the copy constructor
    ThreadPool(const ThreadPool&) = delete;

    // Removing the assignment operator
    ThreadPool& operator=(const ThreadPool&) = delete;

    // Adds a new task to the task queue and returns std::future to get the result
    template <typename Func>
    auto enqueue(Func&& task) -> std::future<decltype(task())> {
        auto pck = std::make_shared<std::packaged_task<decltype(task())()>>(std::forward<Func>(task));
        auto future = pck->get_future();

        {
            std::unique_lock<std::mutex> lock(mutex);
            tasks.emplace([pck]() { (*pck)(); });
        }

        condVar.notify_one();
        return future;
    }

    // Starts processing the task queue
    void WorkOn();

    // Stops the thread pool and waits for all tasks to complete
    void Destroy();

private:
    std::vector<std::thread> threads; // Vector of worker threads
    std::queue<std::function<void()>> tasks; // Task queue
    std::mutex mutex; // Mutex to synchronize access to the task queue
    std::condition_variable condVar; // Condition variable для сигналізації про наявність нових завдань
    bool stop; // Flag to stop the thread pool

    // Loop function for worker threads
    void workerThread();
};