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
#include <memory>
#include <netinet/ip.h>

#include "ThreadPool.h"
#include "TunInterface.h"
#include "Authenticator.h"
#include "Logger.h"

class UDPServer {
public:
    UDPServer(uint16_t port, size_t numThreads);
    ~UDPServer();

    int createSocket();
    void closeSocket(int socketFd);
    bool start();
    void run();

private:
    int socketFd;
    sockaddr_in serverAddr;
    std::unique_ptr<ThreadPool> threadPool;
    std::unique_ptr<TunInterface> tun;
    std::unique_ptr<AuthManager> auth;
    int clientTimeoutInSeconds = 60;
    AES128 aes;
    int MTU = 1500;

    // Logger instance
    ent::util::Logger* logger;

    // Function of processing unauthorized client packets
    void handleGuest(std::string message, const sockaddr_in& clientAddr, socklen_t clientLen);

    // Function of processing authorized client packets
    void handleClient(const int ClientSocket, const sockaddr_in& clientAddr, socklen_t clientLen);

    // Read from tun interface and send packets to clients
    void runReadTun();

    //The function of checking customer activity within 60 seconds
    void runCheckClientActivity();

};
