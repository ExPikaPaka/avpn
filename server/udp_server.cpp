#include "udp_server.h"

UDPServer::UDPServer(uint16_t port, size_t numThreads)
    : threadPool(std::make_unique<ThreadPool>(numThreads)) {
    socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFd == -1) {
        throw std::system_error(errno, std::system_category(), "Failed to create socket");
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

}

UDPServer::~UDPServer() {
    if (socketFd != -1) {
        close(socketFd);
    }
}

bool UDPServer::start() {
    if (bind(socketFd, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        throw std::system_error(errno, std::system_category(), "Failed to bind socket");
    }

    return true;
}

void UDPServer::run() {
    constexpr int bufferSize = 1024;
    char buffer[bufferSize];

    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        ssize_t bytesReceived = recvfrom(socketFd, buffer, bufferSize, 0, (sockaddr*)&clientAddr, &clientLen);
        if (bytesReceived == -1) {
            throw std::system_error(errno, std::system_category(), "Failed to receive data");
        }

        std::string message(buffer, bytesReceived);
        threadPool->enqueue([this, message, clientAddr, clientLen] {
            handleClient(message, clientAddr, clientLen);
        });
    }
}

void UDPServer::handleClient(const std::string& message, const sockaddr_in& clientAddr, socklen_t clientLen) {
    std::cout << "Received message: " << message << std::endl;

    const std::string response = "Hello from server!";
    try {
        ssize_t bytesSent = sendto(socketFd, response.c_str(), response.length(), 0, (const sockaddr*)&clientAddr, clientLen);
        if (bytesSent < 0) {
            throw std::system_error(errno, std::system_category(), "Failed to send response");
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}