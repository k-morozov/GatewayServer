//
// Created by focus on 06.06.2021.
//

#include "QueryEngine.h"
#include "sdk/channels/users/User.h"

namespace goodok {

    void QueryEngine::reg(sessionWeakPtr const& sessionWeak, Serialize::RegistrationRequest const& request)
    {
        db::InputSettings settings{
            .clientName=request.login(),
            .clientPassword=request.password()
        };
        auto client_id = db_->checkRegUser(settings);

        if (client_id != db::REG_LOGIN_IS_BUSY) {
            userPtr userPtr = std::make_shared<User>(sessionWeak, request.login(), request.password());
            userPtr->setId(client_id);
            idClients_[client_id] = userPtr;
            log::write(log::Level::info, "QueryEngine",
                       boost::format("registration new user. login=%1%, client_id=%2%") % userPtr->getName() % userPtr->getId());
        } else {
            log::write(log::Level::warning, "QueryEngine",
                       boost::format("registration failed. user=%1% contains yet.") % request.login());
        }

        if (auto session = sessionWeak.lock()) {
            auto buffer = MsgFactory::serialize<command::TypeCommand::RegistrationResponse>(client_id);
            session->write(buffer);
        }
    }

    void QueryEngine::auth(sessionWeakPtr const& sessionWeak, Serialize::AuthorisationRequest const& request)
    {
        db::InputSettings settings{
                .clientName=request.login(),
                .clientPassword=request.password()
        };
        auto client_id = db_->checkAuthUser(settings);

        if (client_id != db::AUTH_LOGIN_IS_NOT_AVAILABLE) {
            userPtr userPtr = std::make_shared<User>(sessionWeak, request.login(), request.password());
            userPtr->updateSession(sessionWeak);
            idClients_[client_id] = userPtr;
            log::write(log::Level::info, "QueryEngine",
                       boost::format("authorisation user. login=%1%, id=%2%")
                       % userPtr->getName() % userPtr->getId());
        } else {

        }

        if (auto session = sessionWeak.lock()) {
            auto buffer = MsgFactory::serialize<command::TypeCommand::AuthorisationResponse>(client_id);
            session->write(buffer);
        }

    }

    void QueryEngine::getHistory(Serialize::HistoryRequest const& request)
    {
        log::write(log::Level::info, "QueryEngine",
                   boost::format("get history, user=%1%, channel=%2%") % request.client_id() % request.channel_name());
        auto it_channel = nameChannels_.find(request.channel_name());
        if (it_channel != nameChannels_.end()) {
            if (it_channel->second) {
                DateTime since = DateTime(
                        Time(request.since().seconds(), request.since().minutes(), request.since().hours()),
                        Date(request.since().day(), request.since().month(), request.since().year())
                );

                it_channel->second->sendHistory(request.client_id(), since);
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

    void QueryEngine::joinRoom(Serialize::JoinRoomRequest const& request)
    {
        if (!nameChannels_.contains(request.channel_name())) {
            std::size_t id = ++counterId_;
            nameChannels_.insert({request.channel_name(), std::make_shared<Channel>(request.channel_name(), id)});
            log::write(log::Level::info, "QueryEngine",
                       boost::format("generate new channel=%1%, id=%2%.") % request.channel_name() % id);
        }

        if (auto it_channel = nameChannels_.find(request.channel_name()); it_channel!=nameChannels_.end()) {
            std::size_t idClient = request.client_id();
            if (auto it_id_client = idClients_.find(idClient); it_id_client != idClients_.end()) {
                if (it_channel->second) {
                    log::write(log::Level::info, "QueryEngine",
                               boost::format("add new user=%1% to channel=%2%") % it_id_client->second->getName() % it_channel->second->getName());
                    it_channel->second->addUser(it_id_client->second);
                    clientChannels_[it_id_client->second->getId()].push_back(request.channel_name());

//                    for(std::string const& channel : clientChannels_[it_id_client->second->getId()]) {
//                        log::write(log::Level::trace, "QueryEngine",
//                                   boost::format("user=%1% has channel=%2%") % it_id_client->second->getName() % channel);
//                    }
                } else {
                    log::write(log::Level::error, "QueryEngine",
                               boost::format("failed joinRoom. do not find channel=%1% in engine") % request.channel_name());
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

    void QueryEngine::sendText(Serialize::TextRequest const& request)
    {
        command::ClientTextMsg message{
                .author=request.login(),
                .text=request.text(),
                .channel_name=request.channel_name(),
                // @TODO find conversion
                .dt= DateTime(
                        Time(
                                request.datetime().seconds(),
                                request.datetime().minutes(),
                                request.datetime().hours()
                        ),
                        Date(
                            request.datetime().day(),
                            request.datetime().month(),
                            request.datetime().year()
                        )
                )
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
