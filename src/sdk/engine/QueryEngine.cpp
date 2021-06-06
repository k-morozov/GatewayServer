//
// Created by focus on 06.06.2021.
//

#include "QueryEngine.h"

namespace goodok {

    void QueryEngine::reg(sessionWeakPtr const& sessionWeak, Serialize::RegistrationRequest const& request)
    {
        std::size_t id = 0;

        auto userPtr = std::make_shared<detail::UserImpl>(detail::UserImpl{
                .session=sessionWeak,
                .login=request.login(),
                .password="",
                .id=0
        });

        if (auto it = usersData_.find(userPtr); it==usersData_.end()) {
            id = ++counterSession_;
            userPtr->id = id;
            userPtr->password = request.password();
            usersData_.insert(userPtr);
            log::write(log::Level::info, "QueryEngine",
                       boost::format("registration new user. login=%1%, id=%2%") % userPtr->login % userPtr->id);
        } else {
            log::write(log::Level::warning, "QueryEngine",
                       boost::format("registration failed. user=%1% contains yet.") % userPtr->login);
        }

        if (auto session = sessionWeak.lock()) {
            auto buffer = MsgFactory::serialize<command::TypeCommand::RegistrationResponse>(id);
            session->write(buffer);
        }
    }

    void QueryEngine::auth(sessionWeakPtr const& sessionWeak, Serialize::AuthorisationRequest const& request)
    {
        std::size_t id = 0;

        auto userPtr = std::make_shared<detail::UserImpl>(detail::UserImpl{
                .session=sessionWeak,
                .login=request.login(),
                .password=request.password(),
                .id=0
        });

        // @TODO add checks for it
        if (auto it = usersData_.find(userPtr); it!=usersData_.end()) {
            if ((*it)->password == request.password()) {
                log::write(log::Level::info, "QueryEngine",
                           boost::format("authorisation user. login=%1%, id=%2%")
                           % request.login() % (*it)->id);
                id = (*it)->id;
            } else {
                log::write(log::Level::warning, "QueryEngine",
                           boost::format("wrong password. login=%1%") % request.login());
            }

        } else {
            log::write(log::Level::warning, "QueryEngine",
                       boost::format("user not found. login=%1%") % request.login());
        }

        if (auto session = sessionWeak.lock()) {
            auto buffer = MsgFactory::serialize<command::TypeCommand::AuthorisationResponse>(id);
            session->write(buffer);
        }

    }

    void QueryEngine::joinRoom(sessionWeakPtr const& session, Serialize::JoinRoomRequest const& request)
    {

    }
}

#include "QueryEngine.h"
