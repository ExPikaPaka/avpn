#pragma once

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <utility>
#include <memory>
#include <vector>
#include <arpa/inet.h>
#include <ctime>

#include "Diffi_Hellman.h"

class AuthManager {
private:
    // Database. Stores login = password.
    std::unordered_map<std::string, std::string> userdb;

    struct ClientData {
        std::time_t lastAccessTime; // time of client access to the server
        std::string key = ""; // Shared key generated during the Diffie-Hellman procedure
        bool isAuthorized = false; // Is the client authorized
    };

    // Stores clientIP = ClientData
    std::unordered_map<uint32_t, ClientData> authorizedClients;

public:
    // Default constructer. Opens DB file and reads it to the program DB.
    AuthManager(const std::string& filename);

    // Check if client login + password is correct.
    bool authenticate(const std::string& message, const sockaddr_in& clientAddr);

    // Check if client with given IP address is authorized and if the activity time has expired (10 seconds)
    bool isClientAuthorized(const sockaddr_in& clientAddr);

    // Checks whether the Diffie-Hellman procedure was performed for a client with a given IP address
    bool isDiffieHellmanPerformed(const sockaddr_in& clientAddr);

    // Returns the public key of the server for the Diffie-Hellman procedure
    std::string GetPublicServerKey(const std::string& message, const sockaddr_in& clientAddr);

    // Returns the shared key generated during the Diffie-Hellman procedure for the client with this IP address
    std::string GetSharedKey(const sockaddr_in& clientAddr);

private:
    // Loads DB from file
    bool loadUserDB(const std::string& filename);

    // Converts string of 'login password' to pair
    std::pair<std::string, std::string> parseCredentials(const std::string& message);
};