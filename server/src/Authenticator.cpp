#include "Authenticator.h"

AuthManager::AuthManager(const std::string& filename) {
    if (!loadUserDB(filename)) {
        throw std::runtime_error("Failed to load user database");
    }
}

bool AuthManager::authenticate(const std::string& message, const sockaddr_in& clientAddr) {
    AES128 aes;
    std::string decodedText = aes.decrypt(message, getSharedKey(clientAddr));
    std::pair<std::string, std::string> credentials = parseCredentials(decodedText);

    auto it = userdb.find(credentials.first);
    if (it != userdb.end() && it->second == credentials.second) {
        auto iter = authorizedClients.find(clientAddr.sin_addr.s_addr);
        iter->second.lastAccessTime = std::chrono::steady_clock::now();
        iter->second.isAuthorized = true;
        std::cout << "The user " << credentials.first << " is authorized" << std::endl;
        return true;
    }

    std::cout << "Incorrect login or password" << std::endl;
    return false;
}

bool AuthManager::isClientAuthorized(const sockaddr_in& clientAddr) {
    auto it = authorizedClients.find(clientAddr.sin_addr.s_addr);

    if (it == authorizedClients.end()) {
        return false;
    }

    return it->second.isAuthorized;
}

bool AuthManager::isDiffieHellmanPerformed(const sockaddr_in& clientAddr) {
    auto it = authorizedClients.find(clientAddr.sin_addr.s_addr);
    return it != authorizedClients.end() && it->second.key != "";
}

std::string AuthManager::getPublicServerKey(const std::string& message, const sockaddr_in& clientAddr) {
    try {
        std::unique_ptr<Diffi_Hellman> diffiHellman = std::make_unique<Diffi_Hellman>(message);

        // Знайти вільну IP-адресу
        in_addr_t freeIP = findFreeIP();
        if (freeIP == 0) {
            std::cerr << "No free IP addresses available" << std::endl;
            return "No free IP addresses available";
        }

        authorizedClients[clientAddr.sin_addr.s_addr] = {freeIP, std::chrono::steady_clock::now(), diffiHellman->getSharedSecret(), false, -1, &clientAddr};
        std::string publicKeyAndIP = std::to_string(diffiHellman->getPublicKey()) + " " + inet_ntoa(*(in_addr*)&freeIP);
        return publicKeyAndIP;
    } catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return "Invalid message parameters";
    }
}

std::string AuthManager::getSharedKey(const sockaddr_in& clientAddr) {
    auto it = authorizedClients.find(clientAddr.sin_addr.s_addr);
    if (it == authorizedClients.end()) {
        return 0;
    }
    return it->second.key;
}

int AuthManager::getClientSocket(const sockaddr_in& clientAddr) {
    auto it = authorizedClients.find(clientAddr.sin_addr.s_addr);
    if (it == authorizedClients.end()) {
        return 0;
    }
    return it->second.socketFd;
}

void AuthManager::addClientSocket(const sockaddr_in& clientAddr, int socketFd) {
    auto it = authorizedClients.find(clientAddr.sin_addr.s_addr);
    if (it != authorizedClients.end()) {
        it->second.socketFd = socketFd;
        it->second.publicAddr = &clientAddr;
    } else {
        throw std::runtime_error("Failed add client socket");
    }
}

void AuthManager::checkClientActivity() {
    auto currentTime = std::chrono::steady_clock::now();

    for (auto it = authorizedClients.begin(); it != authorizedClients.end();) {
        auto lastActivityTime = it->second.lastAccessTime;
        if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastActivityTime).count() > 60) {
            it = authorizedClients.erase(it);
        } else {
            ++it;
        }
    }
}

void AuthManager::updateClientActivity(const sockaddr_in& clientAddr) {
    auto it = authorizedClients.find(clientAddr.sin_addr.s_addr);
    if (it != authorizedClients.end()) {
        it->second.lastAccessTime = std::chrono::steady_clock::now();
    } else {
        throw std::runtime_error("Client dont found");
    }
}

sockaddr_in AuthManager::getClientPublicAddr(in_addr_t localIP) {
    for (const auto& [clientAddr, clientData] : authorizedClients) {
        if (clientData.localIP == localIP) {
            return *clientData.publicAddr;
        }
    }
    throw std::runtime_error("Local IP client is not found");
}

bool AuthManager::loadUserDB(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            userdb[line.substr(0, pos)] = line.substr(pos + 1);
        }
    }
    file.close();
    return true;
}

std::pair<std::string, std::string> AuthManager::parseCredentials(const std::string& message) {
    size_t pos = message.find(' ');
    if (pos != std::string::npos) {
        return {message.substr(0, pos), message.substr(pos + 1)};
    }
    return std::make_pair("", "");
}

in_addr_t AuthManager::findFreeIP() {
    in_addr_t startIP = inet_addr("10.0.0.2");
    in_addr_t endIP = inet_addr("10.0.0.255");

    for (in_addr_t ip = ntohl(startIP); ip <= ntohl(endIP); ++ip) {
        in_addr_t currentIP = htonl(ip);
        bool ipUsed = false;

        for (const auto& client : authorizedClients) {
            if (client.second.localIP == currentIP) {
                ipUsed = true;
                break;
            }
        }

        if (!ipUsed) {
            return currentIP;
        }
    }

    return 0;
}