//
// Created by focus on 31.05.2021.
//

#include "sdk/network/session/ClientSession.h"

#include <boost/asio/yield.hpp>

#include <algorithm>

namespace goodok {
    namespace detail {

        SocketWriter::SocketWriter(std::weak_ptr<socket_t> sock) :
            socketWeak_(std::move(sock))
        {
            log::write(log::Level::trace, "SocketWriter", "ctor done");
        }

        void SocketWriter::write(std::string const& message)
        {
            processWrite = !bufferWrite_.empty();
            detail::buffer_t data = detail::convert(message);
            bufferWrite_.push_back(std::move(data));
            if (!processWrite) {
                writeImpl_();
            }
        }

        void SocketWriter::writeImpl_() {
            if (auto socket = socketWeak_.lock()) {
                auto callback = [selfWeak = weak_from_this()](boost::system::error_code,
                                                                                   std::size_t) {
                    log::write(log::Level::info, "SendHandler", "ok");
                    if (auto self = selfWeak.lock()) {
                        self->bufferWrite_.pop_front();
                        if (!self->bufferWrite_.empty()) {
                            self->writeImpl_();
                        }
                    }
                };
                if (socket) {
                    boost::asio::async_write(
                            *socket,
                            boost::asio::buffer(bufferWrite_.front()),
                            std::move(callback));
                } else {
                    log::write(log::Level::error, "SocketWriter", "async_write socket is close");
                }

            } else {
                log::write(log::Level::error, "SocketWriter", "writeImpl_ socket is close");
            }
        }

    }

    ClientSession::ClientSession(AsyncContextWeakPtr ctxWeak, socket_t && socket) :
        ctx_(std::move(ctxWeak)),
        socket_(std::make_shared<socket_t>(std::move(socket))),
        writer_(std::make_shared<detail::SocketWriter>(detail::weak_from(socket_)))
    {
        if (!socket_){
            throw std::invalid_argument("incorrect socket");
        }
        if (!writer_) {
            throw std::invalid_argument("writer_ is failed");
        }
        log::write(log::Level::trace, "ClientSession", "ctor done");
    }

    ClientSession::~ClientSession()
    {
//        @TODO check when socket close
        if (socket_ && socket_->is_open()) {
            socket_->close();
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

        auto task = [](detail::buffer_header_t const& textHeader, detail::buffer_body_t const& textBody)
        {
            log::write(log::Level::debug, "ClientSession",
                       boost::format("read header: size = %1%, text=[%2%]")
                            % textHeader.size() % detail::convert(textHeader));
            log::write(log::Level::debug, "ClientSession",
                       boost::format("read body: size = %1%, text=[%2%]")
                        % textBody.size() % detail::convert(textBody));
        };

        auto callback = [selfWeak = weak_from_this()](boost::system::error_code ec, std::size_t nbytes) mutable {
            if (auto self = selfWeak.lock()) {
                self->runRead(ec, nbytes);
            }
        };

        reenter(coroData_.coro_) for(;;)
        {
            log::write(log::Level::debug, "ClientSession", "read header");
            yield boost::asio::async_read(*socket_,
                                          boost::asio::buffer(coroData_.bufferHeader_.data(), detail::MAX_SIZE_HEADER_BUFFER),
                                          callback);

            log::write(log::Level::debug, "ClientSession", "read body");
            yield boost::asio::async_read(*socket_,
                                          boost::asio::buffer(coroData_.bufferBody_.data(), detail::MAX_SIZE_BODY_BUFFER),
                                          callback);

//             @TODO send new data after read
            AsyncContext::runAsync(ctx_, task, coroData_.bufferHeader_, coroData_.bufferBody_);

            write("ok1\n");
            write("ok2\n");
            write("ok3\n");

        }
    }

    void ClientSession::write(std::string const& message)
    {
        writer_->write(message);
    }

}