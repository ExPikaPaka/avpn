#include "TunInterface.h"

TunInterface::TunInterface(const std::string& interfaceName) {
    tunFd = createTunInterface(interfaceName);
    if (tunFd < 0) {
        throw std::runtime_error("Failed to create TUN interface");
    }
}

TunInterface::~TunInterface() {
    close(tunFd);
}

int TunInterface::read(char* buffer, int bufferSize) {
    return ::read(tunFd, buffer, bufferSize);
}

int TunInterface::write(const char* data, int dataSize) {
    return ::write(tunFd, data, dataSize);
}

int TunInterface::getFd() {
    return tunFd;
}

int TunInterface::createTunInterface(const std::string& interfaceName) {
    int tun_fd = open("/dev/net/tun", O_RDWR);
    if (tun_fd < 0) {
        std::cerr << "Error opening TUN interface" << std::endl;
        return -1;
    }

    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
    std::strncpy(ifr.ifr_name, interfaceName.c_str(), IFNAMSIZ);

    if (ioctl(tun_fd, TUNSETIFF, (void*)&ifr) < 0) {
        std::cerr << "Error configuring TUN interface" << std::endl;
        close(tun_fd);
        return -1;
    }

    return tun_fd;
}