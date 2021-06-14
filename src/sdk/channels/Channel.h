//
// Created by focus on 06.06.2021.
//

#ifndef GOODOK_SERVERS_CHANNEL_H
#define GOODOK_SERVERS_CHANNEL_H

#include "IChannel.h"

#include "sdk/database/IDatabase.h"
#include "sdk/channels/users/IUser.h"
#include "sdk/channels/users/UserManager.h"

#include <string>
#include <list>


namespace goodok {

    class Channel : public IChannel {
    public:
        Channel(std::shared_ptr<UserManager> manager, std::weak_ptr<db::IDatabase> db, std::string const& name, std::size_t id);

        std::size_t getId() const override { return id_; }
        std::string getName() const override { return name_; }

        void addUser(db::type_id_user) override;
        void sendHistory(std::size_t id, DateTime const& dt) override;
        void write(command::ClientTextMsg const&) override;

        ~Channel() = default;
    private:
        std::shared_ptr<UserManager> manager_;
        std::weak_ptr<db::IDatabase> db_;

        const std::string name_;
        const std::size_t id_;

        std::unordered_set<db::type_id_user> idUsers_;
    };

}

#endif //GOODOK_SERVERS_CHANNEL_H
