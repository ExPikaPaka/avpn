#include "TunInterface.h"

TunInterface::TunInterface(const std::string& interfaceName, const std::string& ipAddress, const std::string& netmask) {
    tunFd = createTunInterface(interfaceName);
    if (tunFd < 0) {
        throw std::runtime_error("Failed to create TUN interface");
    }
    configTun(interfaceName, ipAddress, netmask);
}

TunInterface::~TunInterface() {
    close(tunFd);
    std::cout << "close tun" << std::endl;
}

int TunInterface::read(char* buffer, int bufferSize) {
    return ::read(tunFd, buffer, bufferSize);
}

int TunInterface::write(const char* data, int dataSize) {
    return ::write(tunFd, data, dataSize);
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

void TunInterface::configTun(const std::string& interfaceName, const std::string& ipAddress, const std::string& netmask) {
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ipAddress.c_str());

    std::string command = "ifconfig " + interfaceName + " " + ipAddress + " netmask " + netmask;
    system(command.c_str());

    std::string routeCommand = "route add -net 0.0.0.0 netmask 0.0.0.0 dev " + interfaceName;
    system(routeCommand.c_str());

    std::string iptablesCommand = "iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE";
    system(iptablesCommand.c_str());
}
