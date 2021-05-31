//
// Created by focus on 31.05.2021.
//

#ifndef GOODOK_FRONT_SERVER_SESSION_H
#define GOODOK_FRONT_SERVER_SESSION_H

#include "tools/log/Logger.h"

#include <boost/asio.hpp>
#include <boost/asio/coroutine.hpp>


namespace goodok {


    class Session {
    public:
        Session(boost::asio::ip::tcp::socket &&socket);

    private:
        boost::asio::ip::tcp::socket socket_;
        boost::asio::coroutine coroCommunicate_;

        char buffer_[3];
    private:
        void run(boost::system::error_code = {}, std::size_t = 0);
    };

}

#endif //GOODOK_FRONT_SERVER_SESSION_H
