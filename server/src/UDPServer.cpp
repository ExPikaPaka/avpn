#include "UDPServer.h"

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
    char buffer[MTU];
    tun = std::make_unique<TunInterface>("tun0", "10.0.0.1", "255.255.255.0");
    auth = std::make_unique<AuthManager>("res/db/userdb.txt");
    
    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        ssize_t bytesReceived = recvfrom(socketFd, buffer, MTU, 0, (sockaddr*)&clientAddr, &clientLen);
        if (bytesReceived == -1) {
            if (errno != EAGAIN || errno != EWOULDBLOCK) {
                throw std::system_error(errno, std::system_category(), "Failed to receive data");
            }
            continue; 
        }

        std::string message(buffer, bytesReceived);
        handleGuest(message, clientAddr, clientLen);
    }
}

void UDPServer::handleGuest(std::string message, const sockaddr_in& clientAddr, socklen_t clientLen){
    if (auth->isDiffieHellmanPerformed(clientAddr)) {
        if (auth->authenticate(message, clientAddr)) {
            std::string response = "OK";
            sendto(socketFd, response.c_str(), response.length(), 0, (struct sockaddr*)&clientAddr, clientLen);

            // Start threads using lambdas
            std::thread clientToServerThread([&]() {
                runClientToServer(clientAddr, clientLen);
            });

            std::thread serverToClientThread([&]() {
                runServerToClient(clientAddr, clientLen);
            });

            // Join the threads with the main thread
            clientToServerThread.join();
            serverToClientThread.join();
        } else {
            std::string response = "FAIL";
            sendto(socketFd, response.c_str(), response.length(), 0, (struct sockaddr*)&clientAddr, clientLen);
        }
    } else {
        std::string responseKey = auth->GetPublicServerKey(message, clientAddr);
        sendto(socketFd, responseKey.c_str(), responseKey.length(), 0, (struct sockaddr*)&clientAddr, clientLen);
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


void UDPServer::runClientToServer(const sockaddr_in& clientAddr, socklen_t clientLen) {
    while (true) {
        try {
            clientToServer(clientAddr, clientLen);
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }
    
}

void UDPServer::runServerToClient(const sockaddr_in& clientAddr, socklen_t clientLen) {
        while (true) {
        try {
            serverToClient(clientAddr, clientLen);
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }
}

void UDPServer::clientToServer(const sockaddr_in& clientAddr, socklen_t clientLen) {
    char buffer[MTU];
    ssize_t bytesReceived = recvfrom(socketFd, buffer, MTU, 0, (sockaddr*)&clientAddr, &clientLen);
    if (bytesReceived > 0) {
        std::string message(buffer, bytesReceived);
        std::string decodedText = aes.decrypt(message, auth->GetSharedKey(clientAddr));
        tun->write(decodedText.c_str(), decodedText.size());
    }
}

void UDPServer::serverToClient(const sockaddr_in& clientAddr, socklen_t clientLen) {
    char buffer[MTU];
    ssize_t bytesRead = tun->read(buffer, MTU);
    if (bytesRead > 0) {
        std::string message(buffer, bytesRead);
        std::string encodedText = aes.encrypt(message, auth->GetSharedKey(clientAddr));
        sendto(socketFd, encodedText.c_str(), encodedText.size(), 0, (struct sockaddr*)&clientAddr, clientLen);
    }
}
