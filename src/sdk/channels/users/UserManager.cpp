//
// Created by focus on 12.06.2021.
//

#include "UserManager.h"
#include "sdk/common/log/Logger.h"
namespace goodok {

    void UserManager::push(db::type_id_user client_id, userPtr data) {
        if (idClients_.contains(client_id)) {
            // @TODO check other params data with current User
            log::write(log::Level::info, "UserManager",
                       boost::format("update session in client: name=%1%, id=%2%") % data->getName() % client_id);
            idClients_[client_id]->updateSession(data->getSession());
        } else {
            log::write(log::Level::info, "UserManager",
                       boost::format("create new client: name=%1%, id=%2%") % data->getName() % client_id);
            idClients_[client_id] = std::move(data);
        }
    }

    userPtr UserManager::getUser(db::type_id_user client_id) {
        if (auto it=idClients_.find(client_id); it!=idClients_.end()) {
            if (it->second) {
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