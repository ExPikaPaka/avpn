#include <gtest/gtest.h>

#include "../src/UDPServer.h"

TEST(UDPServerTest, CreateSocket) {
    UDPServer server(1994, 4);
    int socketFd = server.createSocket();
    EXPECT_NE(socketFd, -1); // Перевірка, що сокет було успішно створено
    server.closeSocket(socketFd);
}
