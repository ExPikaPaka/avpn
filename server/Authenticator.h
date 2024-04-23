#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <arpa/inet.h>

class AuthManager {
private:
    std::map<std::string, std::string> userdb;
    std::map<uint32_t, std::string> authorizedClients; // Зберігає IP-адреси авторизованих клієнтів та відповідні імена користувачів

public:
    AuthManager(const std::string& filename) {
        loadUserDB(filename);
    }

    bool authenticate(const std::string& message, const sockaddr_in& clientAddr) {
        std::pair<std::string, std::string> credentials = parseCredentials(message);
        std::string username = credentials.first;
        std::string password = credentials.second;

        auto it = userdb.find(username);
        if (it != userdb.end() && it->second == password) {
            std::cout << "Користувач " << username << " авторизований" << std::endl;
            authorizedClients[clientAddr.sin_addr.s_addr] = username; // Додаємо авторизованого клієнта до списку
            return true;
        }

        std::cout << "Невірний логін або пароль" << std::endl;
        return false;
    }

    bool isClientAuthorized(const sockaddr_in& clientAddr) {
        uint32_t clientIP = clientAddr.sin_addr.s_addr;
        return authorizedClients.find(clientIP) != authorizedClients.end();
    }

    std::string getClientUsername(const sockaddr_in& clientAddr) {
        uint32_t clientIP = clientAddr.sin_addr.s_addr;
        auto it = authorizedClients.find(clientIP);
        if (it != authorizedClients.end()) {
            return it->second;
        }
        return "";
    }

private:
    void loadUserDB(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        while (std::getline(file, line)) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                std::string username = line.substr(0, pos);
                std::string password = line.substr(pos + 1);
                userdb[username] = password;
            }
        }
        file.close();
    }

    std::pair<std::string, std::string> parseCredentials(const std::string& message) {
        size_t pos = message.find(' ');
        if (pos != std::string::npos) {
            std::string username = message.substr(0, pos);
            std::string password = message.substr(pos + 1);
            return std::make_pair(username, password);
        }
        return std::make_pair("", "");
    }
};