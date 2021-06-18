//
// Created by focus on 13.06.2021.
//

#ifndef GOODOK_SERVERS_CHANNELSMANAGER_H
#define GOODOK_SERVERS_CHANNELSMANAGER_H

#include "Channel.h"

#include "sdk/database/IDatabase.h"

namespace goodok {

    class ChannelsManager {
    public:
        ChannelsManager(std::weak_ptr<UserManager> manager, std::weak_ptr<db::IDatabase> db);

        channelPtr createOrGetChannelByName(std::string const&);

        bool has(std::string const& channel_name) const;

        channelPtr get(std::string const& channel_name) const;

        std::deque<std::string> getUserNameChannels(db::type_id_user const&) const;

        void joinClientChannel(db::type_id_user, std::string const&) const;
    private:
        std::weak_ptr<UserManager> managerUsers_;
        std::weak_ptr<db::IDatabase> db_;

        std::mutex mutex_;
        std::unordered_map<db::type_id_user, channelPtr> idChannels_;

    private:
        void createChannelFromDb();
    };

}

#endif //GOODOK_SERVERS_CHANNELSMANAGER_H
