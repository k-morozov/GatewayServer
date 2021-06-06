//
// Created by focus on 06.06.2021.
//

#ifndef GOODOK_SERVERS_CHANNEL_H
#define GOODOK_SERVERS_CHANNEL_H

#include <string>
#include <list>


namespace goodok {

    class Channel {
    public:
        Channel(std::string const& name);
        ~Channel() = default;

    private:
        std::size_t id; // @TODO who generate?
        std::string name;

        // @TODO list users in this channels?
        // @TODO new class for history?
        std::list<std::string> history_;
    };

}

#endif //GOODOK_SERVERS_CHANNEL_H
