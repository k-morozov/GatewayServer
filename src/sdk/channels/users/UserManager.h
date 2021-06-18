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
        explicit UserManager(std::weak_ptr<db::IDatabase> db) :
            db_(std::move(db))
        {
        }

        static userPtr create(UserSettings const&);

        void push(userPtr);

        userPtr getUser(db::type_id_user);

        db::type_id_user checkRegUser(db::InputSettings const&) const;
        db::type_id_user checkAuthUser(db::InputSettings const&) const;

    private:
        std::weak_ptr<db::IDatabase> db_;
        std::unordered_map<db::type_id_user, userPtr> idClients_;
    };

}


#endif //GOODOK_SERVERS_USERMANAGER_H
