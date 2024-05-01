#include "AES128.h"

std::string AES128::encrypt(const std::string &plainText, const std::string &key) {
    // Validate key length
    if (key.size() != 16) {
        std::cerr << "Error: Key length must be 16 bytes (128 bits)." << std::endl;
        return "";
    }

    EVP_CIPHER_CTX *ctx;
    int len, cipherTextLen;
    std::string cipherText;

    // Create and initialize encryption context
    ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, (const unsigned char*)key.c_str(), NULL);

    len = plainText.size();
    // Resize cipherText
    cipherText.resize(len + EVP_CIPHER_block_size(EVP_aes_128_ecb()));
    // Encryption
    EVP_EncryptUpdate(ctx, (unsigned char*)&cipherText[0], &len, (const unsigned char*)plainText.c_str(), plainText.size());
    cipherTextLen = len;

    // Finalize encryption
    EVP_EncryptFinal_ex(ctx, (unsigned char*)&cipherText[len], &len);
    cipherTextLen += len;

    EVP_CIPHER_CTX_free(ctx);
    
    // Resize cipherText to actual encrypted data size
    cipherText.resize(cipherTextLen);

    return cipherText;
}

std::string AES128::decrypt(const std::string &encryptedText, const std::string &key) {
    // Validate key length
    if (key.size() != 16) {
        std::cerr << "Error: Key length must be 16 bytes (128 bits)." << std::endl;
        return "";
    }
    
    EVP_CIPHER_CTX *ctx;
    int len, plainTextLen;
    std::string plainText;

    // Create and initialize decryption context
    ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, (const unsigned char*)key.c_str(), NULL);

    len = encryptedText.size();
    // Resize plainText 
    plainText.resize(len + EVP_CIPHER_block_size(EVP_aes_128_ecb()));
    // Decryption
    EVP_DecryptUpdate(ctx, (unsigned char*)&plainText[0], &len, (const unsigned char *)encryptedText.c_str(), encryptedText.size());
    plainTextLen = len;

    // Finalize decryption
    EVP_DecryptFinal_ex(ctx, (unsigned char*)&plainText[len], &len);
    plainTextLen += len;

    EVP_CIPHER_CTX_free(ctx);

    // Resize plainText to actual decrypted data size
    plainText.resize(plainTextLen);
    return plainText;
}