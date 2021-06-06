//
// Created by focus on 03.06.2021.
//

#ifndef GOODOK_SERVERS_ISESSION_H
#define GOODOK_SERVERS_ISESSION_H

#include "protocol/protocol.h"

namespace goodok {

    class ISession;
    using sessionWeakPtr = std::weak_ptr<ISession>;
    using sessionPtr = std::shared_ptr<ISession>;

    class ISession {
    public:
        virtual void startRead() = 0;
        virtual void write(std::vector<uint8_t> const&) = 0;
        virtual ~ISession() = default;
    };

}
#endif //GOODOK_SERVERS_ISESSION_H
