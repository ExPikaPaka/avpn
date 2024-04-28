#include "ThreadPool.h"

ThreadPool::ThreadPool(std::size_t numThreads) {
    stop = false;
    for (std::size_t i = 0; i < numThreads; ++i) {
        threads.emplace_back([this] { this->workerThread(); });
    }
}

ThreadPool::~ThreadPool() {
    if (!stop) {
        Destroy();
    }
}

void ThreadPool::WorkOn() {
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

void ThreadPool::Destroy() {
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

void ThreadPool::workerThread() {
    while (!stop) {
        try {
            WorkOn();
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}