//
// Created by focus on 31.05.2021.
//

#include "sdk/network/session/Session.h"

#include <boost/asio/yield.hpp>

#include <algorithm>

namespace goodok {

    Session::Session(AsyncContextWeakPtr ctxWeak, boost::asio::ip::tcp::socket && socket) :
        ctx_(std::move(ctxWeak)),
        socket_(std::move(socket))
    {
        log::write(log::Level::debug, "Session", "ctor done");
    }

    void Session::start()
    {
        runRead();
    }


    void Session::runRead(boost::system::error_code ec, std::size_t)
    {
        if (ec) {
            log::write(log::Level::error,
                       "Session",
                       boost::format("read with error: %1%") % ec.message());
            return;
        }

        auto task = [](std::string text)
        {
            if (auto it = text.find('\n'); it != std::string::npos)
            {
                text.erase(it);
            }
            log::write(log::Level::debug, "Session", boost::format("read: size = %1%, text=[%2%]") % text.size() % text);
        };

        auto callback = [this](boost::system::error_code ec, std::size_t nbytes) {
            runRead(ec, nbytes);
        };

        reenter(coroData_.coro_) for(;;)
        {
            log::write(log::Level::debug, "Session", "read header");
            yield boost::asio::async_read(socket_,
                                          boost::asio::buffer(coroData_.bufferHeader_, 3),
                                          callback);
            // @TODO prepare headers
            // read body
            log::write(log::Level::debug, "Session", "read body");
            yield boost::asio::async_read(socket_,
                                          boost::asio::buffer(coroData_.bufferBody_, 3),
                                          callback);

            // @TODO send new data after read

//            task("test");
//            std::cout << "result = " << std::is_invocable<decltype(task), std::string>::value << std::endl;
            AsyncContext::runAsync(ctx_, task, coroData_.bufferHeader_);
            AsyncContext::runAsync(ctx_, task, coroData_.bufferBody_);
//            AsyncContext::runAsync(ctx_, task);

//            yield boost::asio::async_write(socket_,
//                                           boost::asio::buffer("ok", 3),
//                                          std::bind(&Session::runReadHeader, this,
//                                                    std::placeholders::_1, std::placeholders::_2));
        }
    }

}