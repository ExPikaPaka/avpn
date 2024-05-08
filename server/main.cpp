#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>

#include "src/UDPServer.h"

int main(int argc, char* argv[]) {
     // Creating logger instance
    ent::util::Logger *logger = ent::util::Logger::getInstance();
    logger->setLogToConsole(true);
    logger->setLogToFile(true);
    logger->setLogLevel(ent::util::level::DEBUG); // Displaying EVERY message. Use INFO for release

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <num_threads>" << std::endl;
        return EXIT_FAILURE;
    }

    try {
        logger->addLog("| Main | Attempting to run server.", ent::util::level::INFO);
        uint16_t port = std::stoi(argv[1]);
        if (port == 0 || port > 65535) {
            throw std::out_of_range("Port number must be in the range [1, 65535]");
        }
        size_t numThreads = std::stoi(argv[2]);
        if (numThreads == 0) {
            throw std::out_of_range("Number of threads must be greater than 0");
        }

        std::unique_ptr<UDPServer> server = std::make_unique<UDPServer>(port, numThreads);

        if (!server->start()) {
            throw std::runtime_error("Failed to start server");
        }

        server->run();
    } catch (const std::exception& e) {
        std::string errMessage = e.what();
        logger->addLog("| Main | Error: " + errMessage, ent::util::level::FATAL);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}