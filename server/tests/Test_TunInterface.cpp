#include <gtest/gtest.h>

#include "../src/TunInterface.h"

TEST(TunInterfaceTest, CreateTunWithValidName) {
    TunInterface tun("tun0");
    EXPECT_GT(tun.getFd(), 0);
}

TEST(TunInterfaceTest, CreateTunWithInvalidName) {
    EXPECT_THROW(TunInterface tun("invalid_name"), std::runtime_error);
}

TEST(TunInterfaceTest, CreateTunWithEmptyName) {
    EXPECT_THROW(TunInterface tun(""), std::runtime_error);
}

TEST(TunInterfaceTest, DestructorClosesDevice) {
    int fd = open("/dev/net/tun", O_RDWR);
    ASSERT_GE(fd, 0);
    {
        TunInterface tun("tun0");
    }
    char buffer;
    EXPECT_EQ(read(fd, &buffer, 1), -1);
    close(fd);
}
