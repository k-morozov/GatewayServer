//
// Created by focus on 12.06.2021.
//

#ifndef GOODOK_SERVERS_IDATABASE_H
#define GOODOK_SERVERS_IDATABASE_H

#include "Settings.h"

#include <protocol/protocol.h>

#include <deque>

namespace goodok::db {

    using type_id_user = std::size_t; // and channel id

    constexpr type_id_user LOGIN_IS_FREE = 0;
    constexpr type_id_user REG_LOGIN_IS_BUSY = 0;
    constexpr type_id_user AUTH_LOGIN_IS_NOT_AVAILABLE = 0;
    constexpr type_id_user AUTH_WRONG_PASSWORD = 0;

    class IDatabase {
    public:
        virtual ~IDatabase() = default;

        virtual bool connect(ConnectSettings const&) = 0;

        virtual type_id_user checkRegUser(InputSettings const&) = 0;
        virtual type_id_user checkAuthUser(InputSettings const&) = 0;
        virtual std::deque<std::string> getUserNameChannels(type_id_user const&) = 0;
        virtual bool hasChannel(std::string const&) const = 0;
        virtual type_id_user createChannel(std::string const&) = 0;
        virtual void joinClientChannel(type_id_user, std::string const&) = 0;
        virtual  void addMsgHistory(type_id_user, command::ClientTextMsg const&) = 0;
        virtual std::deque<command::ClientTextMsg> getHistory(type_id_user) = 0;

        virtual type_id_user getChannelId(std::string const &channel_name) const = 0;
    };
}

#endif //GOODOK_SERVERS_IDATABASE_H
