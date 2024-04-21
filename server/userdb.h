#ifndef USERDB_H
#define USERDB_H

#include "common.h"
#include <string>
#include <map>
#include <netinet/in.h>
#include <string>

struct Client {
    sockaddr_in addr;
    int secret;
    bool authorized;
    std::string username;
};

std::map<std::string, std::string> loadUserDB(const std::string& filename);

#endif