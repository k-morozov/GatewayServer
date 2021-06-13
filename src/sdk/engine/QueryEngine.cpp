//
// Created by focus on 06.06.2021.
//

#include "QueryEngine.h"

#include "sdk/channels/users/User.h"
#include "sdk/database/WrapperPg.h"

namespace goodok {

    QueryEngine::QueryEngine(std::shared_ptr<UserManager> manager) :
        manager_(std::move(manager)),
        db_(std::make_shared<db::WrapperPg>())
    {
        if (!manager_) {
            throw std::invalid_argument("manager pointer is nullptr");
        }
        if (!db_) {
            throw std::invalid_argument("database pointer is nullptr");
        }

        db::ConnectSettings settings;
        if (db_->connect(settings)) {
            log::write(log::Level::info, "QueryEngine",
                       "connect to pgsql successfully");
        } else {
            log::write(log::Level::error, "QueryEngine",
                       "failed connect to pgsql");
            db_ = std::make_shared<db::Storage>();
            if (db_->connect(settings)) {
                log::write(log::Level::info, "QueryEngine",
                           "connect to storage successfully");
            } else {
                throw std::invalid_argument("failed init database/storage");
            }
        }
    }

    void QueryEngine::reg(sessionWeakPtr const& sessionWeak, Serialize::RegistrationRequest const& request)
    {
        db::InputSettings inputSettings {
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
            userPtr clientPtr = UserManager::create(userSettings);
            manager_->push(clientPtr);

            log::write(log::Level::info, "QueryEngine",
                       boost::format("registration new user: login=%1%, client_id=%2%") % clientPtr->getName() % clientPtr->getId());
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
        db::InputSettings inputSettings {
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
            userPtr clientPtr = UserManager::create(userSettings);
            manager_->push(clientPtr);

            log::write(log::Level::info, "QueryEngine",
                       boost::format("authorisation user. login=%1%, id=%2%") % clientPtr->getName() % clientPtr->getId());
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

        // проверить есть ли созданный канал, если нет - проверить, должны ли мы создать его?
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
        log::write(log::Level::info, "QueryEngine",
                   boost::format("get channels, client_id=%1%") % request.client_id());
        auto clientPtr = manager_->getUser(request.client_id());
        if (!clientPtr) {
            log::write(log::Level::error, "QueryEngine::getChannels",
                       boost::format("client is nullptr") % request.client_id());
            return;
        }

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
        channelPtr->addUser(clientPtr->getId());
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
