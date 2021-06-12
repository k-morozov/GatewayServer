//
// Created by focus on 06.06.2021.
//

#ifndef GOODOK_SERVERS_CHANNEL_H
#define GOODOK_SERVERS_CHANNEL_H

#include "sdk/database/IDatabase.h"
#include "sdk/channels/users/IUser.h"

#include <string>
#include <list>


namespace goodok {

    class Channel;
    using channelPtr = std::shared_ptr<Channel>;


    class Channel {
    public:
        Channel(std::weak_ptr<db::IDatabase> db, std::string const& name, std::size_t id);

        std::size_t getId() const { return id_; }
        std::string getName() const { return name_; }

        void addUser(userPtr const& user);
        void sendHistory(std::size_t id, DateTime const& dt);
        void write(command::ClientTextMsg const&);

        ~Channel() = default;
    private:
        std::weak_ptr<db::IDatabase> db_;
        const std::string name_;
        const std::size_t id_; // @TODO who generate?

//        @TODO weak? list?
        std::list<userPtr> usersOnline_; // online users
        std::unordered_map<std::size_t, userPtr> idUsers_; // id->User

    };

}

#endif //GOODOK_SERVERS_CHANNEL_H
