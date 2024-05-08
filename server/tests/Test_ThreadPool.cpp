#include <gtest/gtest.h>
#include <numeric>


#include "../src/ThreadPool.h"

TEST(ThreadPoolTest, EnqueueAndExecuteTask) {
    ThreadPool pool(2);

    std::future<int> future = pool.enqueue([]() { return 42; });

    ASSERT_EQ(future.get(), 42);
}

TEST(ThreadPoolTest, EnqueueMultipleTasks) {
    ThreadPool pool(4);
    std::vector<std::future<int>> futures;

    for (int i = 0; i < 100; ++i) {
        futures.emplace_back(pool.enqueue([i]() { return i * i; }));
    }

    for (int i = 0; i < 100; ++i) {
        ASSERT_EQ(futures[i].get(), i * i);
    }
}

TEST(ThreadPoolTest, ExceptionHandling) {
    ThreadPool pool(1);
    auto future = pool.enqueue([]() {
        throw std::runtime_error("Something went wrong");
    });
    ASSERT_THROW(future.get(), std::runtime_error);
}