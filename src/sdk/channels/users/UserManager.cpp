//
// Created by focus on 12.06.2021.
//

#include "User.h"
#include "UserManager.h"

#include "sdk/common/log/Logger.h"
#include "sdk/common/MakeSharedHelper.h"

namespace goodok {

    userPtr UserManager::create(UserSettings const& settings)
    {
        return std::make_shared<MakeSharedHelper<User>>(settings);
    }

    void UserManager::push(userPtr data) {
        if (!data) {
            log::write(log::Level::error, "UserManager::push", "data is nullptr");
            return;
        }

        if (idClients_.contains(data->getId())) {
            // @TODO check other params data with current User
            log::write(log::Level::info, "UserManager",
                       boost::format("update session in client: name=%1%, id=%2%") % data->getName() % data->getId());
            idClients_[data->getId()]->updateSession(data->getSession());
        } else {
            log::write(log::Level::info, "UserManager",
                       boost::format("create new client: name=%1%, id=%2%") % data->getName() % data->getId());
            idClients_[data->getId()] = std::move(data);
        }
    }

    userPtr UserManager::getUser(db::type_id_user client_id) {
        if (auto it=idClients_.find(client_id); it!=idClients_.end()) {
            if (it->second) {
                log::write(log::Level::info, "UserManager::getUser",
                           boost::format("successfully found user with client_id=%1%") % client_id);
                return it->second;
            } else {
                log::write(log::Level::error, "UserManager::getUser",
                           boost::format("found client_id=%1% but user not valid") % client_id);
            }
        } else {
            log::write(log::Level::error, "UserManager::getUser",
                       boost::format("not found client_id=%1%") % client_id);
        }
        return nullptr;
    }
}