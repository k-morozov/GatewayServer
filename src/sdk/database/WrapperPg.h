//
// Created by focus on 12.06.2021.
//

#ifndef GOODOK_SERVERS_WRAPPERPG_H
#define GOODOK_SERVERS_WRAPPERPG_H

#include "IDatabase.h"

#include <atomic>
#include <unordered_map>

namespace goodok::db {

    constexpr type_id_user REG_LOGIN_IS_BUSY = 0;

    class GeneratorId {
    public:
        GeneratorId() = default;
        type_id_user generate() {
            return ++counterId_;
        }
    private:
        std::atomic<type_id_user> counterId_ = 0;
    };


    class WrapperPg : public IDatabase {
    public:
        ~WrapperPg() override = default;

        bool connect(ConnectSettings const& settings) override;

        type_id_user checkRegUser(std::string const&) override;
    private:
        GeneratorId generator_;
        std::unordered_map<std::string, type_id_user> nameUsers_;
    };
}

#endif //GOODOK_SERVERS_WRAPPERPG_H
