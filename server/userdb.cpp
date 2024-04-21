#include "userdb.h"

#include "common.h"
#include <fstream>

std::map<std::string, std::string> loadUserDB(const std::string& filename) {
    std::map<std::string, std::string> userdb;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string username = line.substr(0, pos);
            std::string password = line.substr(pos + 1);
            userdb[username] = password;
        }
    }
    file.close();
    return userdb;
}