//
// Created by focus on 13.06.2021.
//

#include "ChannelsManager.h"

#include "sdk/common/log/Logger.h"


namespace goodok {

ChannelsManager::ChannelsManager(std::shared_ptr<UserManager> manager, std::weak_ptr<db::IDatabase> db) :
    managerUsers_(std::move(manager)),
    db_(std::move(db))
{
    createChannelFromDb();
}

std::deque<std::string> ChannelsManager::getUserNameChannels(db::type_id_user const& client_id) const
{
    std::deque<std::string> channels;
    if (auto db = db_.lock())
    {
        channels = db->getUserNameChannels(client_id);
    }
    return channels;
}

void ChannelsManager::joinClientChannel(db::type_id_user client_id, std::string const& channel_name) const
{
    if (auto db = db_.lock())
    {
        db->joinClientChannel(client_id, channel_name);
    }
}

void ChannelsManager::createChannelFromDb()
{
    std::unordered_map<db::type_id_user, std::string> channels;
    if (auto db = db_.lock()) {
        channels = db->getCurrentChannels();
    }
    for(auto const& [channel_id, channel_name]: channels) {
        idChannels_[channel_id] = std::make_shared<Channel>(managerUsers_, db_, channel_name, channel_id);
        log::write(log::Level::info, "ChannelsManager",
                   boost::format("generate channel: name=%1%, id=%2%") % channel_name % channel_id);
    }
}

channelPtr ChannelsManager::createOrGetChannelByName(std::string const& channel_name) {
    channelPtr channel;
    auto db = db_.lock();
    if (!db) {
        return channel;
    }
    if (db->hasChannel(channel_name)) {
        log::write(log::Level::info, "ChannelsManager",
                   boost::format("channel exist in db yet: name=%1%") % channel_name);
        auto channel_id = db->getChannelId(channel_name);
        log::write(log::Level::info, "ChannelsManager",
                   boost::format("channel_name=%1% has channel_id=%2% in db") % channel_name % channel_id);

        std::lock_guard g(mutex_);
        if (!idChannels_.contains(channel_id)) {
            log::write(log::Level::info, "ChannelsManager",
                       boost::format("channel need generated: only in manager, name=%1%") % channel_name);
            channel = std::make_shared<Channel>(managerUsers_, db_, channel_name, channel_id);
            idChannels_[channel_id] = channel;
        } else {
            log::write(log::Level::info, "ChannelsManager",
                       boost::format("channel exist in manager yet: name=%1%") % channel_name);
            channel = idChannels_[channel_id];
        }
    } else {
        log::write(log::Level::info, "ChannelsManager",
                   boost::format("channel not exist in db: name=%1%") % channel_name);
        auto channel_id = db->createChannel(channel_name);
        channel = std::make_shared<Channel>(managerUsers_, db_, channel_name, channel_id);

        std::lock_guard g(mutex_);
        idChannels_[channel_id] = channel;
        log::write(log::Level::info, "ChannelsManager",
                   boost::format("channel fully generated: name=%1%") % channel_name);
    }
    return channel;
}

bool ChannelsManager::has(std::string const& channel_name) const
{
    if (auto db = db_.lock())
    {
        db::type_id_user channel_id = db->getChannelId(channel_name);
        return idChannels_.contains(channel_id);
    }
    return false;
}

channelPtr ChannelsManager::get(std::string const& channel_name) const
{
    channelPtr channelPtr;
    if (auto db = db_.lock()) {
        const auto channel_id = db->getChannelId(channel_name);
        // @TODO check error?
        channelPtr = idChannels_.at(channel_id);
    }
    return channelPtr;
}


}