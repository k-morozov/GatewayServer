//
// Created by focus on 31.05.2021.
//

#include "Session.h"

#include <boost/asio/yield.hpp>

namespace goodok {

    Session::Session(AsyncContextWeakPtr ctxWeak, boost::asio::ip::tcp::socket && socket) :
        ctx_(ctxWeak),
        socket_(std::move(socket))
    {
        log::write(log::Level::debug, "Session", "ctor done");
        runRead();
    }


    void Session::runRead(boost::system::error_code, std::size_t)
    {
        auto task = [](std::string const& text)
        {
            log::write(log::Level::debug, "Session", text);
        };
        reenter(coroCommunicate_) for(;;)
        {
            yield boost::asio::async_read(socket_,
                                          boost::asio::buffer(buffer_, 3),
                                          std::bind(&Session::runRead, this,
                                                    std::placeholders::_1, std::placeholders::_2));
            AsyncContext::runAsync(ctx_, task, buffer_);

            yield boost::asio::async_write(socket_,
                                           boost::asio::buffer("ok", 3),
                                          std::bind(&Session::runRead, this,
                                                    std::placeholders::_1, std::placeholders::_2));
        }
    }

}