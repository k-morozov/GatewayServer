//
// Created by focus on 12.06.2021.
//

#ifndef GOODOK_SERVERS_USERMANAGER_H
#define GOODOK_SERVERS_USERMANAGER_H

#include "IUser.h"
#include "sdk/database/IDatabase.h"

#include <unordered_map>

namespace goodok {

    class UserManager {
    public:
        UserManager() = default;
        void push(db::type_id_user, userPtr);
        userPtr getUser(db::type_id_user);
    private:
        std::unordered_map<db::type_id_user, userPtr> idClients_;
    };

}


#endif //GOODOK_SERVERS_USERMANAGER_H
