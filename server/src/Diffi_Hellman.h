#pragma once

#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <stdexcept>
#include <memory>

#include "SHA256.h"

class Diffi_Hellman {
private:
    // Server public key for the Diffie-Hellman protocol
    uint64_t publicServerKey; 

    // Shared secret key generated during the Diffie-Hellman protocol
    std::string SharedSecret; 

public:
    // Initializing the Diffi_Hellman object with the parameters specified in the message
    Diffi_Hellman(const std::string& message);

    // Returns the public key of the server
    uint64_t getPublicKey() const;

    // Returns the shared secret key
    std::string getSharedSecret() const;

private:

    // Calculates (base ^ exp) mod p
    uint64_t modPow(uint64_t base, uint64_t exp, uint64_t p);

    // Splits the message into three parameters: g, p, A
    std::vector<uint64_t> parseParams(const std::string& message);

    // Checks if the parameters g, p, A are valid for the Diffie-Hellman protocol
    bool validateParams(const std::vector<uint64_t>& params);

    // Checks if the number n is prime
    bool isPrime(uint64_t n);

     // Checks if g is a primary root mod p
    bool isPrimitive(uint64_t g, uint64_t p);

    // Decomposes the number n into prime factors
    void factorize(uint64_t n, std::vector<uint64_t>& factors);
};