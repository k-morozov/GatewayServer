//
// Created by focus on 06.06.2021.
//

#ifndef GOODOK_SERVERS_CHANNEL_H
#define GOODOK_SERVERS_CHANNEL_H

#include "sdk/channels/users/IUser.h"
#include <string>
#include <list>


namespace goodok {

    class Channel;
    using channelPtr = std::shared_ptr<Channel>;


    class Channel {
    public:
        Channel(std::string const& name, std::size_t id);

        std::size_t getId() const { return id_; }
        std::string getName() const { return name_; }

        void addUser(userPtr const& user);
        void sendHistory(std::size_t id);
        void write(command::ClientTextMsg const&);

        ~Channel() = default;
    private:
        std::string name_;
        std::size_t id_; // @TODO who generate?

//        @TODO weak? list?
        std::list<userPtr> users_;
        std::unordered_map<std::size_t, userPtr> idUsers_;
        std::deque<command::ClientTextMsg> history_;
    };

}

#endif //GOODOK_SERVERS_CHANNEL_H
