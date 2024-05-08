#include <gtest/gtest.h>

#include "../src/Diffi_Hellman.h"

TEST(Diffi_HellmanTest, EmptyInput) {
    std::string message = "";
    EXPECT_THROW(Diffi_Hellman dh(message), std::invalid_argument);
}

TEST(Diffi_HellmanTest, InsufficientParameters) {
    std::string message_1 = "23 5";
    std::string message_2 = "23";
    EXPECT_THROW(Diffi_Hellman dh(message_1), std::invalid_argument);
    EXPECT_THROW(Diffi_Hellman dh(message_2), std::invalid_argument);
}

TEST(Diffi_HellmanTest, ExcessParameters) {
    std::string message = "23 5 16 7";
    EXPECT_THROW(Diffi_Hellman dh(message), std::invalid_argument);
}

TEST(Diffi_HellmanTest, InvalidParameters) {
    std::string message_1 = "23 -5 16";
    std::string message_2 = "0 5 16";
    std::string message_3 = "1 5 16";
    std::string message_4 = "23 0 16";
    std::string message_5 = "23 1 16";
    std::string message_6 = "23 5 0";
    std::string message_7 = "23 5 24";
    std::string message_8 = "23 5 abc";
    std::string message_9 = "23 23 23";

    EXPECT_THROW(Diffi_Hellman dh(message_1), std::invalid_argument);
    EXPECT_THROW(Diffi_Hellman dh(message_2), std::invalid_argument);
    EXPECT_THROW(Diffi_Hellman dh(message_3), std::invalid_argument);
    EXPECT_THROW(Diffi_Hellman dh(message_4), std::invalid_argument);
    EXPECT_THROW(Diffi_Hellman dh(message_5), std::invalid_argument);
    EXPECT_THROW(Diffi_Hellman dh(message_6), std::invalid_argument);
    EXPECT_THROW(Diffi_Hellman dh(message_7), std::invalid_argument);
    EXPECT_THROW(Diffi_Hellman dh(message_8), std::invalid_argument);
    EXPECT_THROW(Diffi_Hellman dh(message_9), std::invalid_argument);
}

TEST(Diffi_HellmanTest, ExtraSpacesAndDelimiters) {
    std::string message_1 = "23   5   16";
    std::string message_2 = "23,5,16";

    EXPECT_THROW(Diffi_Hellman dh(message_1), std::invalid_argument);
    EXPECT_THROW(Diffi_Hellman dh(message_2), std::invalid_argument);
}