#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <arpa/inet.h>

class AuthManager {
private:
    // Database. Stores login = password.
    std::map<std::string, std::string> userdb;

    // Stores clientIP = login
    std::map<uint32_t, std::string> authorizedClients; 

public:
    // Default constructer. Opens DB file and reads it to the program DB.
    AuthManager(const std::string& filename);

    // Check if client login + password is correct.
    bool authenticate(const std::string& message, const sockaddr_in& clientAddr);

    // Check if client with given IP address is authorized
    bool isClientAuthorized(const sockaddr_in& clientAddr);

    // Returns client login by his IP address
    std::string getClientUsername(const sockaddr_in& clientAddr);

private:
    // Loads DB from file
    void loadUserDB(const std::string& filename);

    // Converts string of 'login password' to pair
    std::pair<std::string, std::string> parseCredentials(const std::string& message);
};