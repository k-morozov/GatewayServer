//
// Created by focus on 06.06.2021.
//

#include "QueryEngine.h"
#include "sdk/channels/users/User.h"

namespace goodok {

    void QueryEngine::reg(sessionWeakPtr const& sessionWeak, Serialize::RegistrationRequest const& request)
    {
        db::InputSettings inputSettings{
            .clientName=request.login(),
            .clientPassword=request.password()
        };
        auto client_id = db_->checkRegUser(inputSettings);

        if (client_id != db::REG_LOGIN_IS_BUSY) {
            UserSettings userSettings {
                .sessionWeak = sessionWeak,
                .name = request.login(),
                .password = request.password(),
                .id = client_id
            };
            userPtr userPtr = UserManager::create(userSettings);
            manager_->push(userPtr);

            log::write(log::Level::info, "QueryEngine",
                       boost::format("registration new user: login=%1%, client_id=%2%") % userPtr->getName() % userPtr->getId());
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
        db::InputSettings inputSettings{
                .clientName=request.login(),
                .clientPassword=request.password()
        };
        auto client_id = db_->checkAuthUser(inputSettings);

        if (client_id != db::AUTH_LOGIN_IS_NOT_AVAILABLE) {
            UserSettings userSettings{
                    .sessionWeak = sessionWeak,
                    .name = request.login(),
                    .password = request.password(),
                    .id = client_id
            };
            userPtr userPtr = UserManager::create(userSettings);
            manager_->push(userPtr);

            log::write(log::Level::info, "QueryEngine",
                       boost::format("authorisation user. login=%1%, id=%2%")
                       % userPtr->getName() % userPtr->getId());
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
        auto clientPtr = manager_->getUser(request.client_id());
        if (auto session = clientPtr->getSession().lock()) {
            auto channels = db_->getUserNameChannels(request.client_id()); // lazy?
            auto buffer = MsgFactory::serialize<command::TypeCommand::ChannelsResponse>(channels);
            session->write(buffer);
        }
    }

    void QueryEngine::joinRoom(Serialize::JoinRoomRequest const& request)
    {
        if (!db_->hasChannel(request.channel_name())) {
            auto channel_id = db_->createChannel(request.channel_name());
            nameChannels_[request.channel_name()] = std::make_shared<Channel>(manager_, db_, request.channel_name(), channel_id);
            log::write(log::Level::info, "QueryEngine",
                       boost::format("generate new channel: name=%1%, id=%2%.") % request.channel_name() % channel_id);
        }

        std::size_t client_id = request.client_id();

        auto channelPtr = nameChannels_[request.channel_name()];
        auto clientPtr = manager_->getUser(client_id);
        if (!clientPtr) {
            log::write(log::Level::error, "QueryEngine",
                       boost::format("failed joinRoom. do not find client_id=%1% in engine") % request.client_id());
            return;
        }
        channelPtr->addUser(clientPtr);
        db_->joinClientChannel(client_id, request.channel_name());
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
