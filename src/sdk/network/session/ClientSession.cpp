//
// Created by focus on 31.05.2021.
//

#include "sdk/network/session/ClientSession.h"

#include <boost/asio/yield.hpp>

#include <algorithm>

namespace goodok {

    ClientSession::ClientSession(AsyncContextWeakPtr ctxWeak, boost::asio::ip::tcp::socket && socket) :
        ctx_(std::move(ctxWeak)),
        socket_(std::move(socket))
    {
        log::write(log::Level::trace, "ClientSession", "ctor done");
    }

    ClientSession::~ClientSession()
    {
        if (socket_.is_open()) {
            socket_.close();
        }

        log::write(log::Level::trace, "ClientSession", "dtor done");
    }

    void ClientSession::startRead()
    {
        runRead();
    }


    void ClientSession::runRead(boost::system::error_code ec, std::size_t)
    {
        if (ec) {
            log::write(log::Level::error,
                       "ClientSession",
                       boost::format("read with error: %1%") % ec.message());
            return;
        }

        auto task = [](std::string textHeader, std::string textBody)
        {
//            textHeader.back() = '\0';
//            textBody.back() = '\0';
            log::write(log::Level::debug, "ClientSession",
                       boost::format("read header: size = %1%, text=[%2%]") % textHeader.size() % textHeader);
            log::write(log::Level::debug, "ClientSession",
                       boost::format("read body: size = %1%, text=[%2%]") % textBody.size() % textBody);
        };

        auto callback = [this](boost::system::error_code ec, std::size_t nbytes) {
            runRead(ec, nbytes);
        };

        reenter(coroData_.coro_) for(;;)
        {
            log::write(log::Level::debug, "ClientSession", "read header");
            yield boost::asio::async_read(socket_,
                                          boost::asio::buffer(coroData_.bufferHeader_, 3),
                                          callback);

            log::write(log::Level::debug, "ClientSession", "read body");
            yield boost::asio::async_read(socket_,
                                          boost::asio::buffer(coroData_.bufferBody_, 3),
                                          callback);

            // @TODO send new data after read
            AsyncContext::runAsync(ctx_, task, coroData_.bufferHeader_, coroData_.bufferBody_);

            write("ok1\n");
            write("ok2\n");
            write("ok3\n");

        }
    }

    void ClientSession::write(std::string message)
    {
        // @TODO thread safe
//        coroData_.bufferWrite_.push_back(message);

//        auto task = [this](std::string const& text) {
//            boost::asio::async_write(
//                    socket_,
//                    boost::asio::buffer(text),
//                    [](boost::system::error_code ec, std::size_t bytes) {
//                        log::write(log::Level::info, "SendHandler", "ok");
//                    });
//        };

        AsyncContext::runAsync(ctx_,
                               &ClientSession::writeImpl_,
                               this, message);
    }

    void ClientSession::writeImpl_(std::string const& message)
    {
        boost::asio::async_write(
                socket_,
                boost::asio::buffer(message),
                [](boost::system::error_code , std::size_t ) {
                    log::write(log::Level::info, "SendHandler", "ok");
                });
    }
}