//
// Created by focus on 06.06.2021.
//

#ifndef GOODOK_SERVERS_IUSER_H
#define GOODOK_SERVERS_IUSER_H

#include "sdk/network/session/ISession.h"

namespace goodok {

    class IUser;
    using userPtr = std::shared_ptr<IUser>;
    using userWeakPtr = std::weak_ptr<IUser>;


    class IUser {
    public:
        virtual sessionWeakPtr getSession() const = 0;
        virtual std::string getName() const = 0;
        virtual std::string getPassword() const = 0;

        virtual void setId(std::size_t) = 0;
        virtual std::size_t getId() const = 0;

        virtual ~IUser() = default;
    };

    struct IUserHash {
        std::size_t operator()(userPtr const& user) const {
            return std::hash<std::string>{}(user->getName());
        }
    };

    struct IUserEqual {
        bool operator()(userPtr const& lhs, userPtr const& rhs) const {
            return lhs->getName() == rhs->getName();
        }
    };
}
#endif //GOODOK_SERVERS_IUSER_H
