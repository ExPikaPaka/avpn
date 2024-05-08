#include <gtest/gtest.h>
#include <thread>

#include "../src/Authenticator.h"

TEST(AuthenticatorTest, LoadUserDB) {
    AuthManager auth("res/db/userdb.txt");

    sockaddr_in invalidClientAddr = {0};
    EXPECT_FALSE(auth.isClientAuthorized(invalidClientAddr));
}

TEST(AuthenticatorTest, IsDiffieHellmanPerformed) {
    AuthManager auth("res/db/userdb.txt");

    sockaddr_in clientAddr = {0};
    EXPECT_FALSE(auth.isDiffieHellmanPerformed(clientAddr));

    std::string dhParams = "23 5 8";
    std::string publicKeyAndIP = auth.getPublicServerKey(dhParams, clientAddr);
    EXPECT_TRUE(auth.isDiffieHellmanPerformed(clientAddr));
}

TEST(AuthenticatorTest, GetPublicServerKey) {
    AuthManager auth("res/db/userdb.txt");

    sockaddr_in clientAddr = {0};
    std::string dhParams = "23 5 8";
    std::string publicKeyAndIP = auth.getPublicServerKey(dhParams, clientAddr);
    EXPECT_NE(publicKeyAndIP, "");
    EXPECT_NE(publicKeyAndIP, "Invalid message parameters");
    EXPECT_NE(publicKeyAndIP, "No free IP addresses available");

    dhParams = "invalidparams";
    publicKeyAndIP = auth.getPublicServerKey(dhParams, clientAddr);
    EXPECT_EQ(publicKeyAndIP, "Invalid message parameters");
}

TEST(AuthenticatorTest, getSharedKey) {
    AuthManager auth("res/db/userdb.txt");

    sockaddr_in clientAddr = {0};
    std::string dhParams = "23 5 8";
    std::string publicKeyAndIP = auth.getPublicServerKey(dhParams, clientAddr);
    
    std::string key = auth.getSharedKey(clientAddr);
    EXPECT_NE(key, "");
}

TEST(AuthenticatorTest, getClientSocket) {
    AuthManager auth("res/db/userdb.txt");

    sockaddr_in clientAddr = {0};
    std::string dhParams = "23 5 8";
    std::string publicKeyAndIP = auth.getPublicServerKey(dhParams, clientAddr);

    int socket = auth.getClientSocket(clientAddr);
    EXPECT_EQ(socket, -1);
}

TEST(AuthenticatorTest, AddAndGetSocket) {
    AuthManager auth("res/db/userdb.txt");

    sockaddr_in clientAddr = {0};
    std::string dhParams = "23 5 8";
    std::string publicKeyAndIP = auth.getPublicServerKey(dhParams, clientAddr);

    int socketFd = auth.getClientSocket(clientAddr);
    EXPECT_EQ(socketFd, -1);
    socketFd = 123;
    auth.addClientSocket(clientAddr, socketFd);
    socketFd = auth.getClientSocket(clientAddr);
    EXPECT_EQ(socketFd, 123);
}

// TEST(AuthenticatorTest, CheckClientActivity) {
//    AuthManager auth("res/db/userdb.txt");

//    sockaddr_in clientAddr = {0};
//    std::string dhParams = "23 5 8";
//    auth.getPublicServerKey(dhParams, clientAddr);

//    std::this_thread::sleep_for(std::chrono::seconds(61));
//    auth.checkClientActivity();
//    EXPECT_FALSE(auth.isClientAuthorized(clientAddr));
// }