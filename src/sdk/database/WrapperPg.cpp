//
// Created by focus on 12.06.2021.
//

#include "WrapperPg.h"
#include "log/Logger.h"

namespace goodok::db {

    bool WrapperPg::connect(ConnectSettings const& settings) {
        log::write(log::Level::info, "WrapperPg",
                   boost::format("start connect to %1%") % settings.host);
        return true;
    }

    type_id_user WrapperPg::checkRegUser(InputSettings const& settings)
    {
        type_id_user id = REG_LOGIN_IS_BUSY;
        if (!nameIdUsers_.contains(settings.clientName)) {
            id = generator_.generate();
            nameIdUsers_[settings.clientName] = id;
            nameSettingsUsers_[id] = settings;
        }
        return id;
    }

    type_id_user WrapperPg::checkAuthUser(InputSettings const& settings)
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

    std::deque<std::string> WrapperPg::getUserNameChannels(type_id_user const& client_id)
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

}
