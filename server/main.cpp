#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>

#include "src/UDPServer.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <num_threads>" << std::endl;
        return EXIT_FAILURE;
    }

    try {
        uint16_t port = std::stoi(argv[1]);
        size_t numThreads = std::stoi(argv[2]);

        std::unique_ptr<UDPServer> server = std::make_unique<UDPServer>(port, numThreads);

        if (!server->start()) {
            std::cerr << "Failed to start server" << std::endl;
            return EXIT_FAILURE;
        }

        std::cout << "Server started on port " << port << std::endl;
        
        server->run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}