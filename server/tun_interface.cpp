#include "tun_interface.h"

int createTunInterface() {
    int tun_fd = open("/dev/net/tun", O_RDWR);
    if (tun_fd < 0) {
        std::cerr << "Помилка відкриття TUN-інтерфейсу" << std::endl;
        return -1;
    }

    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
    std::strncpy(ifr.ifr_name, TUN_INTERFACE_NAME, IFNAMSIZ);

    if (ioctl(tun_fd, TUNSETIFF, (void*)&ifr) < 0) {
        std::cerr << "Помилка налаштування TUN-інтерфейсу" << std::endl;
        close(tun_fd);
        return -1;
    }

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        std::cerr << "Помилка відкриття сокета для налаштування TUN-інтерфейсу" << std::endl;
        close(tun_fd);
        return -1;
    }

    // Налаштування IP-адреси та маски підмережі для TUN-інтерфейсу
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(TUN_IP_ADDRESS);
    std::string command = "ifconfig " + std::string(TUN_INTERFACE_NAME) + " " + TUN_IP_ADDRESS + " netmask " + TUN_NETMASK;
    system(command.c_str()); // Виконання системної команди для налаштування IP-адреси та маски підмережі

    return tun_fd;
}