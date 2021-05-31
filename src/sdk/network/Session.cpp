//
// Created by focus on 31.05.2021.
//

#include "Session.h"

#include <boost/asio/yield.hpp>

namespace goodok {
    Session::Session(boost::asio::ip::tcp::socket && socket) :
        socket_(std::move(socket))
    {
        log::write(log::Level::debug, "Session", "ctor done");
        run();
    }


    void Session::run(boost::system::error_code, std::size_t )
    {
        reenter(coroCommunicate_) for(;;)
        {
            yield boost::asio::async_read(socket_,
                                          boost::asio::buffer(buffer_, 3),
                                          std::bind(&Session::run, this,
                                                    std::placeholders::_1, std::placeholders::_2));

            log::write(log::Level::debug, "Session",
                       boost::format("read: %1%") % buffer_);
            yield boost::asio::async_write(socket_,
                                           boost::asio::buffer("ok", 3),
                                          std::bind(&Session::run, this,
                                                    std::placeholders::_1, std::placeholders::_2));
        }
    }

}