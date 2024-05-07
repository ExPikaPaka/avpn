#pragma once

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <utility>
#include <memory>
#include <vector>
#include <arpa/inet.h>
#include <chrono>

#include "Diffi_Hellman.h"
#include "AES128.h"

class AuthManager {
private:
    // Database. Stores login = password.
    std::unordered_map<std::string, std::string> userdb;

    struct ClientData {
        uint32_t localIP = 0; // IP address assigned by the server
        std::chrono::steady_clock::time_point lastAccessTime; // Time of client access to the server
        std::string key = ""; // Shared key generated during the Diffie-Hellman procedure
        bool isAuthorized = false; // Is the client authorized
        int socketFd = -1; // Client socket
        const sockaddr_in* publicAddr = nullptr; // Pointing to a public address
    };

    // Stores PublicClientIP = ClientData
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
    std::string getPublicServerKey(const std::string& message, const sockaddr_in& clientAddr);

    // Returns the shared key generated during the Diffie-Hellman procedure for the client with this IP address
    std::string getSharedKey(const sockaddr_in& clientAddr);

    // Returns the public address of the client by the local address, if it does not exist, then returns an exception
    sockaddr_in getClientPublicAddr(in_addr_t localIP);
    
    // Adds a client socket to the database
    void addClientSocket(const sockaddr_in& clientAddr, int socketFd);
    
    // Get the client's socket from the database
    int getClientSocket(const sockaddr_in& clientAddr);
    
    // Check the client's activity within 60 seconds, if it is not active, delete it from the database
    void checkClientActivity();
    
    // Update the activity timer
    void updateClientActivity(const sockaddr_in& clientAddr);

private:
    // Loads DB from file
    bool loadUserDB(const std::string& filename);

    // Converts string of 'login password' to pair
    std::pair<std::string, std::string> parseCredentials(const std::string& message);

    // Find a free local address from the address pool
    in_addr_t findFreeIP();
};