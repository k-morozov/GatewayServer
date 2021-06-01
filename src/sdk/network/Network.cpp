//
// Created by focus on 31.05.2021.
//

#include "Network.h"

#include <boost/asio/yield.hpp>

namespace goodok {


    Network::Network(AsyncContextWeakPtr ctxWeak, int port) :
            ctx_(ctxWeak),
            port_(port),
            endpoint_(tcp::v4(), port_),
            acceptor_(networkContext_, endpoint_),
            socket_(networkContext_)
    {
        runAccept();
        log::write(log::Level::debug, "Network", "ctor done");
    }


    Network::~Network()
    {
        networkContext_.stop();
        log::write(log::Level::debug, "Network", "dtor done");
    }

    void Network::run()
    {
        networkContext_.run();
    }


    void Network::runAccept()
    {
        doAccept();
    }

    void Network::doAccept(boost::system::error_code)
    {
        reenter(coroAccept_) for(;;)
        {
            log::write(log::Level::trace, "Network", "start wait accept");
            yield acceptor_.async_accept(socket_,
                                         std::bind(&Network::doAccept, this, std::placeholders::_1));
            log::write(log::Level::debug, "Network", "new connection");
            sessions_.emplace_back(std::make_shared<Session>(ctx_, std::move(socket_)));
            socket_.close();
        }
    }

} // end namespace goodok