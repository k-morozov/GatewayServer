//
// Created by focus on 06.06.2021.
//

#include "Channel.h"

#include "log/Logger.h"

namespace goodok {

    Channel::Channel(std::string const& name, std::size_t id) :
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
        // @TODO get history
        if (!user) {
            log::write(log::Level::error, "Channel", "addUser: user is nullptr");
            return;
        }

        if (auto session = user->getSession().lock()) {
            for(auto const& msg : history_) {
                auto buffer = MsgFactory::serialize<command::TypeCommand::EchoResponse>(msg);
                session->write(buffer);
            }
        }

        if (auto it = std::find(std::begin(users_), std::end(users_),user); it == users_.end())
        {
            users_.push_back(user);
            idUsers_.insert({user->getId(), user});

            log::write(log::Level::error, boost::format("Channel=%1%") % name_,
                       boost::format("add user, name=%1%, id=%2%") % user->getName() % user->getId());
        } else {
            log::write(log::Level::error, boost::format("Channel=%1%") % name_, "update session for user in channel");
        }
        if (auto session = user->getSession().lock()) {
            auto buffer = MsgFactory::serialize<command::TypeCommand::JoinRoomResponse>(name_, true);
            session->write(buffer);
        }
    }

    void Channel::sendHistory(std::size_t client_id)
    {
        if (auto it_user=idUsers_.find(client_id); it_user != idUsers_.end()) {
            if (!it_user->second) {
                log::write(log::Level::error, boost::format("Channel=%1%") % name_,
                    boost::format("failed ptr to user_id=%1%") % client_id);
                return;
            }
            if (auto session = it_user->second->getSession().lock()) {
                auto buffer = MsgFactory::serialize<command::TypeCommand::HistoryResponse>(name_, history_);
                session->write(buffer);
            }

        }
    }


    void Channel::write(command::ClientTextMsg const& message)
    {
        auto buffer = MsgFactory::serialize<command::TypeCommand::EchoResponse>(message);

        for(auto const& user : users_) {
            if (!user) {
                continue;
            }
            log::write(log::Level::error, boost::format("Channel=%1%") % name_,
                       boost::format("send msg=%1% to user=%2%") % message.text % user->getName());
            if (auto session = user->getSession().lock()) {
                session->write(buffer);
            }
        }
        history_.push_back(message);
    }

}