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
    AES128 aes;
    int MTU = 2400;

    void handleClient(std::string message, const sockaddr_in& clientAddr, socklen_t clientLen);
    void handleGuest(std::string message, const sockaddr_in& clientAddr, socklen_t clientLen);

    void runClientToServer(const sockaddr_in& clientAddr, socklen_t clientLen);
    void runServerToClient(const sockaddr_in& clientAddr, socklen_t clientLen);

    void clientToServer(const sockaddr_in& clientAddr, socklen_t clientLen);
    void serverToClient(const sockaddr_in& clientAddr, socklen_t clientLen);

};
