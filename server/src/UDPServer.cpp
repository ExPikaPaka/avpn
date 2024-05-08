#include "UDPServer.h"

UDPServer::UDPServer(uint16_t port, size_t numThreads)
    : threadPool(std::make_unique<ThreadPool>(numThreads)) {
    socketFd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    if (socketFd == -1) {
        throw std::system_error(errno, std::system_category(), "Failed to create socket");
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
}
int UDPServer::createSocket() {
    
    int newSocketFd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    if (newSocketFd == -1) {
        throw std::system_error(errno, std::system_category(), "Failed to create new socket");
    }

    sockaddr_in newSocketAddr;
    memset(&newSocketAddr, 0, sizeof(newSocketAddr));
    newSocketAddr.sin_family = AF_INET;
    newSocketAddr.sin_addr.s_addr = INADDR_ANY;
    newSocketAddr.sin_port = htons(0);

    if (bind(newSocketFd, (struct sockaddr *)&newSocketAddr, sizeof(newSocketAddr)) < 0) {
        throw std::system_error(errno, std::system_category(), "Failed to bind socket");
    }

    return newSocketFd;
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
    tun = std::make_unique<TunInterface>("tun0");
    auth = std::make_unique<AuthManager>("res/db/userdb.txt");

    std::thread serverToClientThread([this]() {
        runReadTun();
    });

    std::thread threadCheckClientActivity([this]() {
        runCheckClientActivity();
    });

    serverToClientThread.detach();
    threadCheckClientActivity.detach();
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
        threadPool->enqueue([this, message, clientAddr, clientLen] {
            handleGuest(message, clientAddr, clientLen);
        });
    }
}

void UDPServer::handleGuest(std::string message, const sockaddr_in& clientAddr, socklen_t clientLen){
    if (auth->isDiffieHellmanPerformed(clientAddr)) {
        if (auth->authenticate(message, clientAddr)) {
            int ClientSocket = createSocket();
            sockaddr_in ClientSocketAddr;
            socklen_t ClientSocketAddrLen = sizeof(ClientSocketAddr);
            getsockname(ClientSocket, (sockaddr*)&ClientSocketAddr, &ClientSocketAddrLen);

            auth->addClientSocket(clientAddr, ClientSocket);
            std::string response = "OK " + std::to_string(ntohs(ClientSocketAddr.sin_port));

            sendto(socketFd, response.c_str(), response.length(), 0, (struct sockaddr*)&clientAddr, clientLen);

            std::thread clientToServerThread([this, &clientAddr, ClientSocket, clientLen]() {
                handleClient(ClientSocket, clientAddr, clientLen);
            });

            clientToServerThread.detach();
        } else {
            std::string response = "FAIL";
            sendto(socketFd, response.c_str(), response.length(), 0, (struct sockaddr*)&clientAddr, clientLen);
        }
    } else {
        std::string responseKey = auth->getPublicServerKey(message, clientAddr) ;
        sendto(socketFd, responseKey.c_str(), responseKey.length(), 0, (struct sockaddr*)&clientAddr, clientLen);
    }
}

void UDPServer::handleClient(const int ClientSocket, const sockaddr_in& clientAddr, socklen_t clientLen) {
    auth->updateClientActivity(clientAddr);
    std::cout << "Client " << inet_ntoa(clientAddr.sin_addr) << " connected" << std::endl;
    char buffer[MTU];
    while (auth->isClientAuthorized(clientAddr)) {
        try {
            ssize_t bytesReceived = recvfrom(ClientSocket, buffer, MTU, 0, (sockaddr*)&clientAddr, &clientLen);
            if (bytesReceived > 0) {
                std::string message(buffer, bytesReceived);
                std::string decodedText = aes.decrypt(message, auth->getSharedKey(clientAddr));
                tun->write(decodedText.c_str(), decodedText.size());
                auth->updateClientActivity(clientAddr);
            }

        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }
    closeSocket(ClientSocket);
    std::cout << "Client " << inet_ntoa(clientAddr.sin_addr) << " disconnected" << std::endl;
}

void UDPServer::runReadTun() {
    char buffer[MTU];
    while (true) {
        try {
            ssize_t bytesRead = tun->read(buffer, MTU);
            if (bytesRead > 0) {
                iphdr* ip = reinterpret_cast<iphdr*>(buffer);
                sockaddr_in clientAddr = auth->getClientPublicAddr(ip->daddr);
                std::string message(buffer, bytesRead);
                std::string encodedText = aes.encrypt(message, auth->getSharedKey(clientAddr));
                sendto(auth->getClientSocket(clientAddr), encodedText.c_str(), encodedText.size(), 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
            }
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }
}

void UDPServer::runCheckClientActivity() {
    while (true) {
        auth->checkClientActivity();
        std::this_thread::sleep_for(std::chrono::seconds(60));
    }
}