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
    explicit ThreadPool(std::size_t numThreads)
        : stop(false) {
        for (std::size_t i = 0; i < numThreads; ++i) {
            threads.emplace_back([this] { this->workerThread(); });
        }
    }

    ~ThreadPool() {
        if (!stop) {
            Destroy();
        }
    }

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

    void WorkOn() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(mutex);
                condVar.wait(lock, [this] { return stop || !tasks.empty(); });
                if (stop && tasks.empty()) {
                    return;
                }
                task = std::move(tasks.front());
                tasks.pop();
            }
            task();
        }
    }

    void Destroy() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            stop = true;
        }
        condVar.notify_all();
        for (std::thread& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex mutex;
    std::condition_variable condVar;
    bool stop;

    void workerThread() {
        while (!stop) {
            try {
                WorkOn();
            }
            catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    }
};