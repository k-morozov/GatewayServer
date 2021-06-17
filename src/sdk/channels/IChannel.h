//
// Created by focus on 13.06.2021.
//

#ifndef GOODOK_SERVERS_ICHANNEL_H
#define GOODOK_SERVERS_ICHANNEL_H

#include "sdk/database/IDatabase.h"
#include "sdk/channels/users/IUser.h"

namespace goodok {

    class IChannel;
    using channelPtr = std::shared_ptr<IChannel>;

    class IChannel {
    public:
        virtual std::size_t getId() const = 0;
        virtual std::string getName() const = 0;
        virtual void addUser(db::type_id_user) = 0;
        virtual void sendHistory(std::size_t id, DateTime const& dt) = 0;
        virtual void write(command::ClientTextMsg &&)  = 0;
        virtual ~IChannel() = default;
    };
}
#endif //GOODOK_SERVERS_ICHANNEL_H
