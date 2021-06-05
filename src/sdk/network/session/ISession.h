//
// Created by focus on 03.06.2021.
//

#ifndef GOODOK_SERVERS_ISESSION_H
#define GOODOK_SERVERS_ISESSION_H

namespace goodok {

    class ISession {
    public:
        virtual void startRead() = 0;
        virtual void write(std::string const&) = 0;
//        virtual std::weak_ptr<ISession> weak_from_this() = 0;
        virtual ~ISession() = default;
    };

}
#endif //GOODOK_SERVERS_ISESSION_H
