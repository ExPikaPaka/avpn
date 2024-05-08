#include <gtest/gtest.h>

#include "../src/SHA256.h"

TEST(SHA256Test, HashEmptyString) {
    SHA256 sha;
    std::string data = "";
    std::array<uint8_t, 32> digest = sha.digest();
    
    EXPECT_EQ(SHA256::toString64(digest), "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}