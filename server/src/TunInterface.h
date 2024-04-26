#pragma once

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <fcntl.h>

class TunInterface {
public:
    // Basic constructor. Creates TUN interface with specified name, and configures
    TunInterface(const std::string& interfaceName, const std::string& ipAddress, const std::string& netmask);

    // Destructor. Closes TUN interface
    ~TunInterface();

    // Read data from the TUN device to the buffer
    int read(char* buffer, int bufferSize);

    // Write data to the buffer from the TUN device
    int write(const char* data, int dataSize);

private:
    // TUN descriptor (it is a device file on Linux located at /dev/net/tun) 
    int tunFd; 

    // Creates TUN device with specified name
    int createTunInterface(const std::string& interfaceName);

    // Configures TUN device
    void configTun(const std::string& interfaceName, const std::string& ipAddress, const std::string& netmask);
};