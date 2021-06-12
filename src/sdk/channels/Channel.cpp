//
// Created by focus on 06.06.2021.
//

#include "Channel.h"

#include "log/Logger.h"

namespace goodok {

    Channel::Channel(std::shared_ptr<UserManager> manager, std::weak_ptr<db::IDatabase> db, std::string const& name, std::size_t id) :
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
    }

    void Channel::addUser(userPtr const& user)
    {
        if (!user) {
            log::write(log::Level::error, "Channel", "addUser: user is nullptr");
            return;
        }

        if (auto it = std::find(std::begin(usersOnline_), std::end(usersOnline_),user); it == usersOnline_.end())
        {
            usersOnline_.push_back(user);
            idUsers_.insert(user->getId()); // @TODO save to db

            log::write(log::Level::error, boost::format("Channel=%1%") % name_,
                       boost::format("add user: name=%1%, id=%2%") % user->getName() % user->getId());
        } else {
            log::write(log::Level::error, boost::format("Channel=%1%") % name_, "update session for user in channel");
        }
        if (auto session = user->getSession().lock()) {
            auto buffer = MsgFactory::serialize<command::TypeCommand::JoinRoomResponse>(name_, true);
            session->write(buffer);
        }
    }

    void Channel::sendHistory(std::size_t client_id, DateTime const& dt)
    {
        if (!idUsers_.contains(client_id)) {
            log::write(log::Level::error, boost::format("Channel=%1%") % name_,
                       boost::format("user_id=%1% not used channel=%2%") % client_id % name_);
            return;
        }

        auto clientPtr = manager_->getUser(client_id);
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
                       boost::format("count msg to send user = %1%") % responseHistory.size());
            for(const auto& response : responseHistory) {
                log::write(log::Level::error, boost::format("Channel=%1%") % name_,
                           boost::format("text = %1%") % response.text);
            }
            if (!responseHistory.empty()) {
                auto buffer = MsgFactory::serialize<command::TypeCommand::HistoryResponse>(name_, responseHistory);
                session->write(buffer);
            } else {
                log::write(log::Level::error, boost::format("Channel=%1%") % name_,
                           "have not got a new messages");
            }
        } else {
            log::write(log::Level::warning, boost::format("Channel=%1%") % name_,
                       "session dead");
        }
    }


    void Channel::write(command::ClientTextMsg const& message)
    {
        auto buffer = MsgFactory::serialize<command::TypeCommand::EchoResponse>(message);

        for(auto const& user : usersOnline_) {
            if (!user) {
                continue;
            }
            log::write(log::Level::error, boost::format("Channel=%1%") % name_,
                       boost::format("send msg=%1% to user=%2%") % message.text % user->getName());
            if (auto session = user->getSession().lock()) {
                session->write(buffer);
            }
        }
//        history_.push_back(message);
        if (auto db = db_.lock()) {
            db->addMsgHistory(id_, message);
        }
    }

}