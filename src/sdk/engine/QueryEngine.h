//
// Created by focus on 06.06.2021.
//

#ifndef GOODOK_SERVERS_QUERYENGINE_H
#define GOODOK_SERVERS_QUERYENGINE_H

#include <protocol/protocol.h>

#include "sdk/network/session/ISession.h"
#include "sdk/common/log/Logger.h"

#include <unordered_set>

namespace goodok {

    namespace detail {
        struct UserImpl {
            sessionWeakPtr session;
            std::string login;
            std::string password;
            std::size_t id;
        };

        struct UserHash {
            std::size_t operator()(UserImpl const& user) const {
                return std::hash<std::string>{}(user.login);
            }
        };

        struct UserEqual {
            bool operator()(UserImpl const& lhs, UserImpl const& rhs) const {
                return lhs.login == rhs.login;
            }
        };
    }

    class QueryEngine;
    using enginePtr = std::shared_ptr<QueryEngine>;
    using engineWeakPtr = std::weak_ptr<QueryEngine>;

    class QueryEngine {
    public:
        QueryEngine() = default;
        ~QueryEngine() = default;

        void reg(sessionWeakPtr session, Serialize::RegistrationRequest const& request);
        void auth(sessionWeakPtr session, Serialize::AuthorisationRequest const& request);
    private:
        // @TODO boost::uid?
        std::atomic<std::size_t> counterSession_ = 0;

        std::unordered_set<detail::UserImpl, detail::UserHash, detail::UserEqual> usersData_;
    };
}

#endif //GOODOK_SERVERS_QUERYENGINE_H
