#pragma once

#include <cstdint>
#include <string>
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <system_error>
#include <unistd.h>
#include <thread>
#include <vector>
#include <mutex>
#include <memory>

#include "thread_pool.h"

class UDPServer {
public:
    UDPServer(uint16_t port, size_t numThreads);
    ~UDPServer();

    bool start();
    void run();

private:
    int socketFd;
    sockaddr_in serverAddr;
    std::unique_ptr<ThreadPool> threadPool;

    void handleClient(const std::string& message, const sockaddr_in& clientAddr, socklen_t clientLen);
};