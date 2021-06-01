//
// Created by focus on 31.05.2021.
//

#ifndef GOODOK_FRONT_SERVER_NETWORK_H
#define GOODOK_FRONT_SERVER_NETWORK_H

#include "sdk/context/AsyncContext.h"
#include "Session.h"

#include "tools/log/Logger.h"

#include <boost/asio.hpp>
#include <boost/asio/coroutine.hpp>

namespace goodok {

    class Network {
        using io_context = boost::asio::io_context;
        using tcp = boost::asio::ip::tcp;
    public:
        Network(AsyncContextWeakPtr ctx, int port = 7777);
        ~Network();

        void run();

    private:
        AsyncContextWeakPtr ctx_;
        int port_;

        io_context networkContext_;
        boost::asio::coroutine coroAccept_;

        std::vector<std::shared_ptr<Session>> sessions_;

        tcp::endpoint endpoint_;
        tcp::acceptor acceptor_;
        tcp::socket socket_;

    private:
        void runAccept();
        void doAccept(boost::system::error_code = {});
    };

} // end namespace goodok

#endif //GOODOK_FRONT_SERVER_NETWORK_H
