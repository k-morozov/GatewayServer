//
// Created by focus on 12.06.2021.
//

#include "Storage.h"
#include "log/Logger.h"

namespace goodok::db {

    bool Storage::connect(ConnectSettings const&) {
        log::write(log::Level::info, "Storage",
                   "without network connect");
        return true;
    }

    type_id_user Storage::checkRegUser(InputSettings const& settings)
    {
        type_id_user id = REG_LOGIN_IS_BUSY;
        if (!nameIdUsers_.contains(settings.clientName)) {
            id = generator_.generate();
            nameIdUsers_[settings.clientName] = id;
            nameSettingsUsers_[id] = settings;
        }
        return id;
    }

    type_id_user Storage::checkAuthUser(InputSettings const& settings)
    {
        type_id_user id = AUTH_LOGIN_IS_NOT_AVAILABLE;
        if (nameIdUsers_.contains(settings.clientName)) {
            auto temp_id = nameIdUsers_[settings.clientName];
            if (auto it = nameSettingsUsers_.find(temp_id); it != nameSettingsUsers_.end()) {
                if (it->second.clientPassword == settings.clientPassword) {
                    id = temp_id;
                } else {
                    log::write(log::Level::warning, boost::format("WrapperPg::%1%") % "checkAuthUser",
                               boost::format("wrong password. login=%1%") % settings.clientName);
                }
            } else {
                log::write(log::Level::warning, boost::format("WrapperPg::%1%") % "checkAuthUser",
                           "internal inconsistency");
            }
        } else {
            log::write(log::Level::warning, boost::format("WrapperPg::%1%") % "checkAuthUser",
                       boost::format("user not found. login=%1%") % settings.clientName);
        }

        return id;
    }

    std::deque<std::string> Storage::getUserNameChannels(type_id_user const& client_id)
    {
        std::deque<std::string> result;
        if (auto it = clientChannels_.find(client_id); it != clientChannels_.end()) {
            result = it->second;
        } else {
            log::write(log::Level::error, boost::format("WrapperPg::%1%") % "getUserNameChannels",
                       boost::format("user=%1% have not channels") % client_id);
        }

        return result;
    }

    bool Storage::hasChannel(std::string const& channelName) const
    {
        return nameIdChannels_.contains(channelName);
    }

    type_id_user Storage::createChannel(std::string const& channelName)
    {
        type_id_user id = {};
        if (!hasChannel(channelName)) {
            id = generator_.generate();
            nameIdChannels_[channelName] = id;
        }
        return nameIdChannels_[channelName];
    }

    void Storage::joinClientChannel(type_id_user client_id, std::string const& channel_name)
    {
        clientChannels_[client_id].push_back(channel_name);
    }

    void Storage::addMsgHistory(type_id_user channel_id, command::ClientTextMsg const& msg)
    {
        history_[channel_id].push_back(msg);
    }

    std::deque<command::ClientTextMsg> Storage::getHistory(type_id_user channel_id)
    {
        std::deque<command::ClientTextMsg> result;
        if (auto it = history_.find(channel_id); it != history_.end()) {
            result = it->second;
        }
        return result;
    }

}
