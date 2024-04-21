#include "common.h"
#include "tun_interface.h"
#include "userdb.h"

#include <iostream>
#include <string.h>
#include <map>
#include <vector>
#include <cstring>
#include <fstream>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <math.h>

std::vector<Client> clients; // Список підключених клієнтів

// Функція для обчислення степеня за модулем
int modPow(int base, int exp, int mod) {
    int result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp % 2 == 1)
            result = (result * base) % mod;
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    return result;
}

int main() {
    // Створення сокета
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0) {
        std::cerr << "Помилка створення сокета" << std::endl;
        return 1;
    }

    // Налаштування адреси сервера
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    // Прив'язка сокета до адреси
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Помилка прив'язки сокета" << std::endl;
        close(server_socket);
        return 1;
    }

    // Створення TUN-інтерфейсу
    int tun_fd = createTunInterface();
    if (tun_fd < 0) {
        return 1;
    }

    std::map<std::string, std::string> userdb = loadUserDB("userdb.txt");

    std::cout << "Сервер запущено на порту " << SERVER_PORT << std::endl;

    while (true) {
        char buffer[BUFFER_SIZE];
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        // Отримання повідомлення від клієнта
        ssize_t bytes_received = recvfrom(server_socket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr*)&client_addr, &client_addr_len);
        if (bytes_received < 0) {
            std::cerr << "Помилка отримання повідомлення" << std::endl;
            continue;
        }
        buffer[bytes_received] = '\0';
        std::string message(buffer);
        // Пошук клієнта в списку
        Client* client = nullptr;
        for (auto& c : clients) {
            if (c.addr.sin_addr.s_addr == client_addr.sin_addr.s_addr) {
                client = &c;
                break;
            }
        }

        // Якщо клієнт новий, виконати протокол Діффі-Геллмана
        if (!client) {
            Client new_client;
            new_client.addr = client_addr;
            new_client.authorized = false;

            // Генерація секретного значення для клієнта
            int client_secret = std::rand() % PRIME;
            new_client.secret = modPow(BASE, client_secret, PRIME);

            std::string response = "HELLO " + std::to_string(PRIME) + " " + std::to_string(BASE) + " " + std::to_string(new_client.secret);
            sendto(server_socket, response.c_str(), response.length(), 0, (struct sockaddr*)&client_addr, client_addr_len);

            clients.push_back(new_client);
        }
        // Якщо клієнт вже був підключений
        else {
            // Якщо клієнт не авторизований, виконати авторизацію
            if (!client->authorized) {
                size_t pos = message.find(' ');
                if (pos != std::string::npos) {
                    std::string username = message.substr(0, pos);
                    std::string password = message.substr(pos + 1);

                    // Перевірка логіну та пароля
                    auto it = userdb.find(username);
                    if (it != userdb.end() && it->second == password) {
                        int server_secret = std::rand() % PRIME;
                        int shared_secret = modPow(client->secret, server_secret, PRIME);

                        std::string response = "OK " + std::to_string(modPow(BASE, server_secret, PRIME));
                        sendto(server_socket, response.c_str(), response.length(), 0, (struct sockaddr*)&client_addr, client_addr_len);

                        client->authorized = true;
                        client->username = username;
                        std::cout << "Користувач " << username << " авторизований" << std::endl;
                    }
                    else {
                        std::string response = "FAIL";
                        sendto(server_socket, response.c_str(), response.length(), 0, (struct sockaddr*)&client_addr, client_addr_len);
                    }
                }
            }
            // Якщо клієнт авторизований, обробляти повідомлення
            else {
                std::string response = "Hello " + client->username + ": " + message;
                write(tun_fd, response.c_str(), response.length());

                // Зчитування даних з TUN-інтерфейсу та відправка клієнту
                bytes_received = read(tun_fd, buffer, BUFFER_SIZE - 1);
                if (bytes_received > 0) {
                    buffer[bytes_received] = '\0';
                    sendto(server_socket, buffer, bytes_received, 0, (struct sockaddr*)&client_addr, client_addr_len);
                }
            }
        }
    }

    close(server_socket);
    close(tun_fd);
    return 0;
}