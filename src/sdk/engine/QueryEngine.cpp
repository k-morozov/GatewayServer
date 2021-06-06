//
// Created by focus on 06.06.2021.
//

#include "QueryEngine.h"

namespace goodok {

    void QueryEngine::reg(sessionWeakPtr const& sessionWeak, Serialize::RegistrationRequest const& request)
    {
        std::size_t id = 0;
        detail::UserImpl user{
            .session=sessionWeak,
            .login=request.login(),
            .password="",
            .id=0
        };
        if (!usersData_.contains(user)) {
            id = ++counterSession_;
            user.id = id;
            user.password = request.password();
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

    void QueryEngine::auth(sessionWeakPtr const& sessionWeak, Serialize::AuthorisationRequest const& request)
    {
        detail::UserImpl user{
            .session=sessionWeak,
            .login=request.login(),
            .password=request.password(),
            .id=0
        };
        if (auto it = usersData_.find(user); it!=usersData_.end()) {
            if (it->password == request.password()) {
                log::write(log::Level::info, "QueryEngine",
                           boost::format("authorisation user. login=%1%, id=%2%")
                           % request.login() % it->id);
                user.id = it->id;
            } else {
                log::write(log::Level::warning, "QueryEngine",
                           boost::format("wrong password. login=%1%") % request.login());
            }

        } else {
            log::write(log::Level::warning, "QueryEngine",
                       boost::format("user not found. login=%1%") % request.login());
        }

        if (auto session = sessionWeak.lock()) {
            auto buffer = MsgFactory::serialize<command::TypeCommand::AuthorisationResponse>(user.id);
            session->write(buffer);
        }

    }
}

#include "QueryEngine.h"
