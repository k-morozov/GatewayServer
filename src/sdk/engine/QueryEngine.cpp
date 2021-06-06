//
// Created by focus on 06.06.2021.
//

#include "QueryEngine.h"

namespace goodok {

    void QueryEngine::reg(sessionWeakPtr sessionWeak, Serialize::RegistrationRequest const& request)
    {
        std::size_t id = 0;
        detail::UserImpl user{sessionWeak, request.login(), ""};
        if (!usersData_.contains(user)) {
            id = ++counterSession_;
            user.id = id;
            usersData_.insert(std::move(user));
            log::write(log::Level::info, "QueryEngine",
                       boost::format("registration new user. login=%1%, id=%2%") % request.login() % id);
        } else {
            log::write(log::Level::warning, "QueryEngine",
                       boost::format("registration failed. user=%1% contains yet.") % request.login());
        }

        if (auto session = sessionWeak.lock()) {
            auto buffer = MsgFactory::serialize<command::TypeCommand::RegistrationResponse>(id);
            session->write(buffer);
        }
    }

    void QueryEngine::auth(sessionWeakPtr session, Serialize::AuthorisationRequest const& request)
    {

    }
}

#include "QueryEngine.h"
