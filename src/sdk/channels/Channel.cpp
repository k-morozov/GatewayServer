//
// Created by focus on 06.06.2021.
//

#include "Channel.h"

#include "log/Logger.h"

namespace goodok {

    Channel::Channel(std::weak_ptr<UserManager> manager, std::weak_ptr<db::IDatabase> db, std::string const& name, std::size_t id) :
        manager_(std::move(manager)),
        db_(std::move(db)),
        name_(name),
        id_(id)
    {
        if (name_.empty()) {
            throw std::invalid_argument("Channel: empty name");
        }
        if (id == 0) {
            throw std::invalid_argument("Channel: id equal zero");
        }

        if (auto db = db_.lock()) {
            auto users_id = db->getChannelUsers(id_);

            if (users_id.empty()) {
                log::write(log::Level::warning, boost::format("Channel=%1%") % name_,
                           "no users in db");
            }
            for(auto const& user_id : users_id) {
                log::write(log::Level::debug, boost::format("Channel=%1%") % name_,
                           boost::format("add user: client_id=%1%") % user_id);
                idUsers_.insert(user_id);
            }
        }
        log::write(log::Level::debug, boost::format("Channel=%1%") % name_,
                   "created");
    }

    void Channel::addUser(db::type_id_user client_id)
    {
        auto manager = manager_.lock();
        if (!manager) {
            return;
        }

        if (!idUsers_.contains(client_id)) {
            idUsers_.insert(client_id);
            auto user = manager->getUser(client_id);
            if (!user) {
                log::write(log::Level::error, boost::format("Channel=%1%") % name_,
                           boost::format("add user: not valid user with id=%1%") % client_id);
                return;
            }

            log::write(log::Level::info, boost::format("Channel=%1%") % name_,
                       boost::format("add user: name=%1%, id=%2%") % user->getName() % user->getId());

            if (auto session = user->getSession().lock()) {
                auto buffer = MsgFactory::serialize<command::TypeCommand::JoinRoomResponse>(name_, true);
                session->write(std::move(buffer));
            }
        }

    }

    void Channel::sendHistory(std::size_t client_id, DateTime const& dt)
    {
        auto manager = manager_.lock();
        if (!manager) {
            return;
        }

        if (!idUsers_.contains(client_id)) {
            log::write(log::Level::error, boost::format("Channel=%1%") % name_,
                       boost::format("user_id=%1% not used channel=%2%") % client_id % name_);
            return;
        }

        auto clientPtr = manager->getUser(client_id);
        if (!clientPtr) {
            log::write(log::Level::error, boost::format("Channel=%1%") % name_,
                boost::format("failed ptr to user_id=%1%") % client_id);
            return;
        }

        if (auto session = clientPtr->getSession().lock()) {
            std::deque<command::ClientTextMsg> responseHistory;
            if (auto db = db_.lock()) {
                auto history = db->getHistory(id_);
                std::copy_if(history.begin(), history.end(), std::back_inserter(responseHistory),
                             [dt](command::ClientTextMsg const& msg) {
                    return dt == DateTime() || dt < msg.dt;
                });
            }
            log::write(log::Level::info, boost::format("Channel=%1%") % name_,
                       boost::format("count messages to send user = %1%") % responseHistory.size());
            if (!responseHistory.empty()) {
                auto buffer = MsgFactory::serialize<command::TypeCommand::HistoryResponse>(name_, responseHistory);
                session->write(std::move(buffer));
            } else {
                log::write(log::Level::error, boost::format("Channel=%1%") % name_,
                           "have not got a new messages");
            }
        } else {
            log::write(log::Level::warning, boost::format("Channel=%1%") % name_,
                       "session dead");
        }
    }


    void Channel::write(command::ClientTextMsg && message)
    {
        auto manager = manager_.lock();
        if (!manager) {
            return;
        }

        auto buffer = MsgFactory::serialize<command::TypeCommand::EchoResponse>(message);

        for(auto const& id : idUsers_) {
            auto user = manager->getUser(id);
            if (!user) {
                continue;
            }
            log::write(log::Level::error, boost::format("Channel=%1%") % name_,
                       boost::format("send msg=%1% to user=%2%") % message.text % user->getName());
            if (auto session = user->getSession().lock()) {
                session->write(std::move(buffer));
            }
        }
        if (auto db = db_.lock()) {
            db->addMsgHistory(id_, std::move(message));
        }
    }

}