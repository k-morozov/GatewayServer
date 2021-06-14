//
// Created by focus on 13.06.2021.
//

#ifndef GOODOK_SERVERS_WRPPARPG_H
#define GOODOK_SERVERS_WRPPARPG_H

#include "IDatabase.h"

#include <postgresql/libpq-fe.h>

namespace goodok::db {

    class WrapperPg : public IDatabase {
    public:
        WrapperPg() = default;
        ~WrapperPg() override = default;

        bool connect(ConnectSettings const&) override;

        type_id_user checkRegUser(InputSettings const&) override;

        type_id_user checkAuthUser(InputSettings const&) override;

        std::deque<std::string> getUserNameChannels(type_id_user const&) override;

        std::vector<type_id_user> getChannelUsers(type_id_user const&) const override;

        bool hasChannel(std::string const&) const override;

        type_id_user createChannel(std::string const&) override;

        void joinClientChannel(type_id_user, std::string const&) override;

// *********************************************************************************************



        void addMsgHistory(type_id_user, command::ClientTextMsg const&) override;

        std::deque<command::ClientTextMsg> getHistory(type_id_user) override;

        std::unordered_map<type_id_user, std::string> getCurrentChannels() const override;
    private:
        PGconn *connection;
        bool isConnected = false;
        mutable std::mutex mutex_;

    public:
        type_id_user getChannelId(std::string const &channel_name) const override;
    private:
        type_id_user getClientId(std::string const &client_name) const;

        std::string getClientName(type_id_user) const;
        std::string getChannelName(type_id_user) const;
        bool hasChannelClient(type_id_user, type_id_user) const;

        PGresult * execThreadSafe(std::string const& query) const;
    };

}
#endif //GOODOK_SERVERS_WRPPARPG_H
