//
// Created by focus on 06.06.2021.
//

#ifndef GOODOK_SERVERS_CHANNEL_H
#define GOODOK_SERVERS_CHANNEL_H

#include "sdk/channels/users/IUser.h"
#include <string>
#include <list>


namespace goodok {

    class Channel {
    public:
        Channel(std::string const& name);

        std::size_t getId() const { return id_; }
        std::string getName() const { return name_; }

        void addUser(userPtr const& user);

        ~Channel() = default;
    private:
        std::size_t id_; // @TODO who generate?
        std::string name_;

        std::list<userPtr> users_;
        std::list<std::string> history_;
    };

}

#endif //GOODOK_SERVERS_CHANNEL_H
