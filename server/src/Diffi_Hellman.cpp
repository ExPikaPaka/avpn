#include "Diffi_Hellman.h"

Diffi_Hellman::Diffi_Hellman(const std::string& message) {
    std::vector<uint64_t> params = parseParams(message);
    if (!validateParams(params)) {
        throw std::invalid_argument("Invalid message parameters");
    }

    uint64_t p = params[0];
    uint64_t g = params[1];
    uint64_t publicClientKey = params[2];

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis(2, p - 1);
    uint64_t privateServerKey = dis(gen);
    publicServerKey = modPow(g, privateServerKey, p);
    sharedSecret = modPow(publicClientKey, privateServerKey, p);
}

uint64_t Diffi_Hellman::modPow(uint64_t base, uint64_t exp, uint64_t p) {
    uint64_t result = 1;
    base %= p;
    while (exp > 0) {
        if (exp % 2 == 1)
            result = (result * base) % p;
        exp = exp >> 1;
        base = (base * base) % p;
    }
    return result;
}

uint64_t Diffi_Hellman::getPublicKey() const {
    return publicServerKey;
}

uint64_t Diffi_Hellman::getSharedSecret() const {
    return sharedSecret;
}

std::vector<uint64_t> Diffi_Hellman::parseParams(const std::string& message) {
    std::vector<uint64_t> params;
    size_t pos = 0;
    while (pos < message.length()) {
        size_t spacePos = message.find(' ', pos);
        if (spacePos == std::string::npos) {
            params.push_back(std::stoull(message.substr(pos)));
            break;
        }
        params.push_back(std::stoull(message.substr(pos, spacePos - pos)));
        pos = spacePos + 1;
    }
    return params;
}

bool Diffi_Hellman::validateParams(const std::vector<uint64_t>& params) {
    if (params.size() != 3) {
        return false;
    }

    uint64_t p = params[0];
    uint64_t g = params[1];
    uint64_t publicClientKey = params[2];

    if (!isPrime(p)) {
        return false;
    }

    if (!isPrimitive(g, p)) {
        return false;
    }

    if (publicClientKey < 2 || publicClientKey >= p) {
        return false;
    }

    return true;
}

bool Diffi_Hellman::isPrime(uint64_t n) {
    if (n <= 1) {
        return false;
    }
    if (n <= 3) {
        return true;
    }
    if (n % 2 == 0 || n % 3 == 0) {
        return false;
    }

    for (uint64_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }

    return true;
}

bool Diffi_Hellman::isPrimitive(uint64_t g, uint64_t p) {
    if (g < 2 || g >= p) {
        return false;
    }

    std::vector<uint64_t> factors;
    factorize(p - 1, factors); 

    for (uint64_t factor : factors) {
        uint64_t power = (p - 1) / factor;
        if (modPow(g, power, p) == 1) {
            return false;
        }
    }

    return true;
}

void Diffi_Hellman::factorize(uint64_t n, std::vector<uint64_t>& factors) {
    uint64_t factor = 2;
    while (n > 1) {
        if (n % factor == 0) {
            factors.push_back(factor);
            n /= factor;
        } else {
            factor++;
        }
    }
}