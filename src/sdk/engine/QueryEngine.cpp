//
// Created by focus on 06.06.2021.
//

#include "QueryEngine.h"
#include "sdk/channels/users/User.h"

namespace goodok {

    void QueryEngine::reg(sessionWeakPtr const& sessionWeak, Serialize::RegistrationRequest const& request)
    {
        std::size_t id = 0;
        userPtr userPtr = std::make_shared<User>(sessionWeak, request.login(), request.password());

        if (auto it = usersData_.find(userPtr); it==usersData_.end()) {
            id = ++counterSession_;
            userPtr->setId(id);
            usersData_.insert(userPtr);
            idClients_[userPtr->getId()] = userPtr;
            log::write(log::Level::info, "QueryEngine",
                       boost::format("registration new user. login=%1%, id=%2%") % userPtr->getName() % userPtr->getId());
        } else {
            log::write(log::Level::warning, "QueryEngine",
                       boost::format("registration failed. user=%1% contains yet.") % userPtr->getName());
        }

        if (auto session = sessionWeak.lock()) {
            auto buffer = MsgFactory::serialize<command::TypeCommand::RegistrationResponse>(id);
            session->write(buffer);
        }
    }

    void QueryEngine::auth(sessionWeakPtr const& sessionWeak, Serialize::AuthorisationRequest const& request)
    {
        std::size_t id = 0;
        userPtr userPtr = std::make_shared<User>(sessionWeak, request.login(), request.password());

        // @TODO add checks for it
        if (auto it = usersData_.find(userPtr); it!=usersData_.end()) {
            if ((*it)->getPassword() == request.password()) {
                log::write(log::Level::info, "QueryEngine",
                           boost::format("authorisation user. login=%1%, id=%2%")
                           % request.login() % (*it)->getId());
                id = (*it)->getId();
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
        /*
         * 1. id channels in engine
         * 2. class Channels
         */
    }
}

#include "QueryEngine.h"
