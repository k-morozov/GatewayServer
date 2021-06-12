//
// Created by focus on 12.06.2021.
//

#ifndef GOODOK_SERVERS_IDATABASE_H
#define GOODOK_SERVERS_IDATABASE_H

#include "Settings.h"

namespace goodok::db {

    class IDatabase {
    public:
        virtual ~IDatabase() = default;

        virtual bool connect(ConnectSettings const&) = 0;
    };
}

#endif //GOODOK_SERVERS_IDATABASE_H
