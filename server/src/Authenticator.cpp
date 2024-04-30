#include "Authenticator.h"

AuthManager::AuthManager(const std::string& filename) {
    if (!loadUserDB(filename)) {
        throw std::runtime_error("Failed to load user database");
    }
}

bool AuthManager::authenticate(const std::string& message, const sockaddr_in& clientAddr) {
    std::pair<std::string, std::string> credentials = parseCredentials(message);

    auto it = userdb.find(credentials.first);
    if (it != userdb.end() && it->second == credentials.second) {
        auto iter = authorizedClients.find(clientAddr.sin_addr.s_addr);
        iter->second.lastAccessTime = std::time(nullptr);
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

    // Check if the activity time has expired (10 seconds)
    std::time_t currentTime = std::time(nullptr);
    std::time_t lastActivityTime = it->second.lastAccessTime;
    if (difftime(currentTime, lastActivityTime) > 10) {
        authorizedClients.erase(it);
        std::cout << "Client disconnect" << std::endl;
        return false;
    }
    it->second.lastAccessTime = std::time(nullptr);

    return it->second.isAuthorized;
}

bool AuthManager::isDiffieHellmanPerformed(const sockaddr_in& clientAddr) {
    auto it = authorizedClients.find(clientAddr.sin_addr.s_addr);
    return it != authorizedClients.end() && it->second.key != "";
}

std::string AuthManager::GetPublicServerKey(const std::string& message, const sockaddr_in& clientAddr) {
    try {
        std::unique_ptr<Diffi_Hellman> diffiHellman = std::make_unique<Diffi_Hellman>(message);

        authorizedClients[clientAddr.sin_addr.s_addr] = {std::time(nullptr), diffiHellman->getSharedSecret(), false};

        return std::to_string(diffiHellman->getPublicKey());
    } catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return "Invalid message parameters";
    }   
}

std::string AuthManager::GetSharedKey(const sockaddr_in& clientAddr){
    auto it = authorizedClients.find(clientAddr.sin_addr.s_addr);
    if (it == authorizedClients.end()) {
        return 0;
    }
    return it->second.key;
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