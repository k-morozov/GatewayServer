//
// Created by focus on 12.06.2021.
//

#ifndef GOODOK_SERVERS_WRAPPERPG_H
#define GOODOK_SERVERS_WRAPPERPG_H

#include "IDatabase.h"

namespace goodok::db {

    class WrapperPg : public IDatabase {
    public:
        ~WrapperPg() override = default;

        bool connect(ConnectSettings const& settings) override;
    private:
    };
}

#endif //GOODOK_SERVERS_WRAPPERPG_H
