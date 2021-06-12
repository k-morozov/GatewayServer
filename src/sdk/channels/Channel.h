//
// Created by focus on 06.06.2021.
//

#ifndef GOODOK_SERVERS_CHANNEL_H
#define GOODOK_SERVERS_CHANNEL_H

#include "sdk/database/IDatabase.h"
#include "sdk/channels/users/IUser.h"
#include "sdk/channels/users/UserManager.h"

#include <string>
#include <list>


namespace goodok {

    class Channel;
    using channelPtr = std::shared_ptr<Channel>;


    class Channel {
    public:
        Channel(std::shared_ptr<UserManager> manager, std::weak_ptr<db::IDatabase> db, std::string const& name, std::size_t id);

        std::size_t getId() const { return id_; }
        std::string getName() const { return name_; }

        void addUser(db::type_id_user);
        void sendHistory(std::size_t id, DateTime const& dt);
        void write(command::ClientTextMsg const&);

        ~Channel() = default;
    private:
        std::shared_ptr<UserManager> manager_;
        std::weak_ptr<db::IDatabase> db_;

        const std::string name_;
        const std::size_t id_; // @TODO who generate?

        std::unordered_set<db::type_id_user> idUsers_;
    };

}

#endif //GOODOK_SERVERS_CHANNEL_H
