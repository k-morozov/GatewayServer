//
// Created by focus on 03.06.2021.
//

#ifndef GOODOK_SERVERS_ISESSION_H
#define GOODOK_SERVERS_ISESSION_H

namespace goodok {

    class ISession {
    public:
        virtual void start() = 0;
    };
}
#endif //GOODOK_SERVERS_ISESSION_H
