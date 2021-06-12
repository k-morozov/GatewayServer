//
// Created by focus on 12.06.2021.
//

#ifndef GOODOK_SERVERS_SETTINGS_H
#define GOODOK_SERVERS_SETTINGS_H

#include <string>

namespace goodok::db {
    struct ConnectSettings {
        std::string user = "worker";
        std::string password = "123";
        std::string host = "127.0.0.1";
        std::string db = "chat";
    };

    struct InputSettings {
        std::string clientName;
        std::string clientPassword;
    };
}
#endif //GOODOK_SERVERS_SETTINGS_H
