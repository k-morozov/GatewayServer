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
    constexpr type_id_user AUTH_LOGIN_IS_NOT_AVAILABLE = 0;
    constexpr type_id_user AUTH_WRONG_PASSWORD = 0;

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

        type_id_user checkRegUser(InputSettings const&) override;

        type_id_user checkAuthUser(InputSettings const&) override;
    private:
        GeneratorId generator_;
        std::unordered_map<std::string, type_id_user> nameIdUsers_;
        std::unordered_map<type_id_user, InputSettings> nameSettingsUsers_;
    };
}

#endif //GOODOK_SERVERS_WRAPPERPG_H
