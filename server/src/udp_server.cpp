#include "udp_server.h"

UDPServer::UDPServer(uint16_t port, size_t numThreads)
    : threadPool(std::make_unique<ThreadPool>(numThreads)) {
    socketFd = createSocket(port);
}
int UDPServer::createSocket(uint16_t port) {
    
    int socket_fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    if (socket_fd == -1) {
        throw std::system_error(errno, std::system_category(), "Failed to create socket");
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    return socket_fd;
}

void UDPServer::closeSocket(int socketFd) {
    if (socketFd != -1) {
        close(socketFd);
        std::cout << "Socket closed" << std::endl;
    }
}

UDPServer::~UDPServer() {
    closeSocket(socketFd);
}

bool UDPServer::start() {
    if (bind(socketFd, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        throw std::system_error(errno, std::system_category(), "Failed to bind socket");
    }

    return true;
}

void UDPServer::run() {
    constexpr int bufferSize = 32767;
    char buffer[bufferSize];
    tun = std::make_unique<TunInterface>("tun0", "10.0.0.1", "255.255.255.0");
    auth = std::make_unique<AuthManager>("userdb.txt");
    
    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        ssize_t bytesReceived = recvfrom(socketFd, buffer, bufferSize, 0, (sockaddr*)&clientAddr, &clientLen);
        if (bytesReceived == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            } else {
                throw std::system_error(errno, std::system_category(), "Failed to receive data");
            }
        }

        std::string message(buffer, bytesReceived);

        if (!auth->isClientAuthorized(clientAddr)) {
            if (auth->authenticate(message, clientAddr)) {
                std::string response = "OK";
                sendto(socketFd, response.c_str(), response.length(), 0, (struct sockaddr*)&clientAddr, clientLen);
            } else {
                std::string response = "FAIL";
                sendto(socketFd, response.c_str(), response.length(), 0, (struct sockaddr*)&clientAddr, clientLen);
            }
        } else {
            threadPool->enqueue([this, message, clientAddr, clientLen] {
                handleClient(message, clientAddr, clientLen);
            });
        }
    }
}

void UDPServer::handleClient(std::string message, const sockaddr_in& clientAddr, socklen_t clientLen) {
    constexpr int bufferSize = 32767;
    char buffer[bufferSize];
    tun->write(message.c_str(), message.length());

    ssize_t bytesRead;
    do {
        bytesRead = tun->read(buffer, bufferSize);
        if (bytesRead > 0) {
            sendto(socketFd, buffer, bytesRead, 0, (struct sockaddr*)&clientAddr, clientLen);
        }
    } while (bytesRead == bufferSize); 
}