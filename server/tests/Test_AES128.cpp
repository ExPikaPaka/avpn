#include <gtest/gtest.h>

#include "../src/AES128.h"

TEST(AES128Test, EncryptDecrypt) {
    AES128 aes;
    std::string key = "0123456789abcdef";
    std::string plaintext = "Hello, World!";

    std::string ciphertext = aes.encrypt(plaintext, key);
    std::string decrypted = aes.decrypt(ciphertext, key);

    EXPECT_EQ(plaintext, decrypted);
}