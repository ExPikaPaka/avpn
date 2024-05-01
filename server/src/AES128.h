#pragma once

#include <iostream>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <cstring>
#include <sstream>
#include <iomanip>

/*
    AES128 class for encrypting and decrypting data using AES-128 ECB mode.
    This implementation uses OpenSSL library.
*/
class AES128 {
public:
    /*
        Encrypts the given plaintext using AES-128 ECB mode with the provided key.
        Returns the encrypted data as a hexadecimal string.
        Key must be 16 bytes (128 bits) long.
    */
    std::string encrypt(const std::string &plainText, const std::string &key);
    /*
        Decrypts the given ciphertext using AES-128 ECB mode with the provided key.
        Returns the decrypted plaintext.
        Key must be 16 bytes (128 bits) long.
    */
    std::string decrypt(const std::string &encryptedText, const std::string &key);
};