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
            id = ++counterId_;
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
                (*it)->updateSession(sessionWeak);
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

    void QueryEngine::getHistory(Serialize::HistoryRequest const& request)
    {
        auto it_channel = nameChannels_.find(request.channel_name());
        if (it_channel != nameChannels_.end()) {
            if (it_channel->second) {
                it_channel->second->sendHistory(request.client_id());
            } else {
                log::write(log::Level::error, "QueryEngine",
                           boost::format("ptr to channel=%1% failed") % request.channel_name());
            }
        } else {
            log::write(log::Level::error, "QueryEngine",
                       boost::format("channel=%1% does not create yet.") % request.channel_name());
        }
    }

    void QueryEngine::getChannels(Serialize::ChannelsRequest const& request)
    {
        if (clientChannels_.contains(request.client_id())) {
            if (auto it_user = idClients_.find(request.client_id()); it_user != idClients_.end()) {
                if (it_user->second) {
                    if (auto session = it_user->second->getSession().lock()) {
                        const auto& channels = clientChannels_[request.client_id()];
                        auto buffer = MsgFactory::serialize<command::TypeCommand::ChannelsResponse>(channels);
                        session->write(buffer);
                    }
                }
            }
        } else {
            log::write(log::Level::error, "QueryEngine",
                       boost::format("user=%1% have not channels") % request.client_id());
        }
    }

    void QueryEngine::joinRoom(sessionWeakPtr const& /*session*/, Serialize::JoinRoomRequest const& request)
    {
        if (!nameChannels_.contains(request.channel_name())) {
            std::size_t id = ++counterId_;
            nameChannels_.insert({request.channel_name(), std::make_shared<Channel>(request.channel_name(), id)});
            log::write(log::Level::info, "QueryEngine",
                       boost::format("generate new id=%1% for channel.") % id);
        }

        if (auto it_channel = nameChannels_.find(request.channel_name()); it_channel!=nameChannels_.end()) {
            std::size_t idClient = request.client_id();
            if (auto it_id_client = idClients_.find(idClient); it_id_client != idClients_.end()) {
                if (it_channel->second) {
                    log::write(log::Level::info, "QueryEngine",
                               boost::format("add new user=%1% to channel=%2%") % it_id_client->second->getName() % it_channel->second->getName());
                    it_channel->second->addUser(it_id_client->second);
                    clientChannels_[it_id_client->second->getId()].push_back(request.channel_name());
                }
            } else {
                log::write(log::Level::error, "QueryEngine",
                           boost::format("failed joinRoom. do not fine client_id=%1% in engine") % request.client_id());
            }
        } else {
            log::write(log::Level::error, "QueryEngine",
                       boost::format("failed joinRoom. channel_name=%1%") % request.channel_name());
        }

    }

    void QueryEngine::sendText(sessionWeakPtr const& /*session*/, Serialize::TextRequest const& request)
    {
//        msg_text_t
        command::ClientTextMsg message{
                .author=request.login(),
                .text=request.text(),
                .channel_name=request.channel_name(),
                .dt={} //request.datetime()
        };

        auto it_channel = nameChannels_.find(request.channel_name());
        if (it_channel != nameChannels_.end()) {
            if (it_channel->second) {
                it_channel->second->write(message);
            } else {
                log::write(log::Level::error, "QueryEngine",
                           boost::format("ptr to channel=%1% failed") % request.channel_name());
            }
        } else {
            log::write(log::Level::error, "QueryEngine",
                       boost::format("channel=%1% does not create yet.") % request.channel_name());
        }
    }
}

#include "QueryEngine.h"
