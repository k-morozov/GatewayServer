//
// Created by focus on 06.06.2021.
//

#ifndef GOODOK_SERVERS_QUERYENGINE_H
#define GOODOK_SERVERS_QUERYENGINE_H

#include <protocol/protocol.h>

#include "sdk/network/session/ISession.h"
#include "sdk/channels/users/IUser.h"
#include "sdk/channels/users/UserManager.h"
#include "sdk/channels/ChannelsManager.h"
#include "sdk/channels/Channel.h"
#include "sdk/common/log/Logger.h"
#include "sdk/database/Storage.h"

#include <unordered_set>

namespace goodok {

    class QueryEngine;
    using enginePtr = std::shared_ptr<QueryEngine>;
    using engineWeakPtr = std::weak_ptr<QueryEngine>;

    class QueryEngine {
    public:
        explicit QueryEngine(std::shared_ptr<UserManager> manager, std::shared_ptr<ChannelsManager> managerChannels);
        ~QueryEngine() = default;

    public:
        void reg(sessionWeakPtr const& session, Serialize::RegistrationRequest const& request);
        void auth(sessionWeakPtr const& session, Serialize::AuthorisationRequest const& request);
        void getHistory(Serialize::HistoryRequest const& request);
        void getChannels(Serialize::ChannelsRequest const& request);
        void joinRoom(Serialize::JoinRoomRequest const& request);
        void sendText(Serialize::TextRequest const& request);
    private:
        std::shared_ptr<UserManager> managerUsers_;
        std::shared_ptr<ChannelsManager> managerChannels_;
    };
}

#endif //GOODOK_SERVERS_QUERYENGINE_H
