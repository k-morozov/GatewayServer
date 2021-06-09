//
// Created by focus on 06.06.2021.
//

#ifndef GOODOK_SERVERS_QUERYENGINE_H
#define GOODOK_SERVERS_QUERYENGINE_H

#include <protocol/protocol.h>

#include "sdk/network/session/ISession.h"
#include "sdk/channels/users/IUser.h"
#include "sdk/channels/Channel.h"
#include "sdk/common/log/Logger.h"

#include <unordered_set>

namespace goodok {

    class QueryEngine;
    using enginePtr = std::shared_ptr<QueryEngine>;
    using engineWeakPtr = std::weak_ptr<QueryEngine>;

    /** @TODO
     * notification user(response) only one time + error checkers
     */
    class QueryEngine {
    public:
        QueryEngine() = default;
        ~QueryEngine() = default;

    public:
        void reg(sessionWeakPtr const& session, Serialize::RegistrationRequest const& request);
        void auth(sessionWeakPtr const& session, Serialize::AuthorisationRequest const& request);
        void getHistory(Serialize::HistoryRequest const& request);
        void getChannels(Serialize::ChannelsRequest const& request);
        void joinRoom(Serialize::JoinRoomRequest const& request);
        void sendText(Serialize::TextRequest const& request);
    private:
        // @TODO boost::uid?
        std::atomic<std::size_t> counterId_ = 0;

        std::unordered_set<userPtr, IUserHash, IUserEqual> usersData_;
        std::unordered_map<std::size_t, userPtr> idClients_;

        std::unordered_map<std::size_t, std::deque<std::string>> clientChannels_;
        std::unordered_map<std::string, channelPtr> nameChannels_;
    };
}

#endif //GOODOK_SERVERS_QUERYENGINE_H
