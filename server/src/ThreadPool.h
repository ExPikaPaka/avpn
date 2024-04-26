#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <future>

// TODO: (From Nick)
// - add description to the class and fucntions. I have no time to 
//   decide how it works and what it does.

class ThreadPool {
public:
    explicit ThreadPool(std::size_t numThreads);

    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

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

    void WorkOn();

    void Destroy();

private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex mutex;
    std::condition_variable condVar;
    bool stop;

    void workerThread();
};