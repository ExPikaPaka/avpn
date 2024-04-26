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

#include "ThreadPool.h"
#include "TunInterface.h"
#include "Authenticator.h"

class UDPServer {
public:
    UDPServer(uint16_t port, size_t numThreads);
    ~UDPServer();

    int createSocket(uint16_t port);
    void closeSocket(int socketFd);
    bool start();
    void run();

private:
    int socketFd;
    sockaddr_in serverAddr;
    std::unique_ptr<ThreadPool> threadPool;
    std::unique_ptr<TunInterface> tun;
    std::unique_ptr<AuthManager> auth;

    void handleClient(std::string message, const sockaddr_in& clientAddr, socklen_t clientLen);

    struct Client {
    sockaddr_in addr;
    bool authorized;
    };
    std::vector<Client> clients;
};
