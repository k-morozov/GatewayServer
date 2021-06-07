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
        if (!user) {
            log::write(log::Level::error, "Channel", "addUser: user is nullptr");
            return;
        }
        if (auto it = std::find(std::begin(users_), std::end(users_),user); it == users_.end())
        {
            users_.push_back(user);
        } else {
            log::write(log::Level::error, "Channel", "update session for user in channel");
        }
        if (auto session = user->getSession().lock()) {
            auto buffer = MsgFactory::serialize<command::TypeCommand::JoinRoomResponse>(name_, true);
            session->write(buffer);
        }
    }

}