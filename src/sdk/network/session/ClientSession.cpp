//
// Created by focus on 31.05.2021.
//



#include "sdk/network/session/ClientSession.h"

#include <boost/asio/yield.hpp>

#include <algorithm>

namespace goodok {
    namespace detail {

        SocketWriter::SocketWriter(std::weak_ptr<ThreadSafeQueue> queue, std::weak_ptr<socket_t> sock) :
            socketWeak_(std::move(sock)),
            queue_(std::move(queue))
        {
            log::write(log::Level::trace, "SocketWriter", "ctor done");
        }

        void SocketWriter::write(std::vector<uint8_t> && message)
        {
           auto task = [selfWeak = weak_from_this(), message{std::move(message)}]() mutable
            {
                if (auto self = selfWeak.lock()) {
                    self->writeImpl_(std::move(message));
                } else {
                    log::write(log::Level::warning, "SocketWriter", "is dead");
                }
            };

            if (auto queue = queue_.lock()) {
                queue->push(std::move(task));
            }
        }

        void SocketWriter::writeImpl_(std::vector<uint8_t> && message) {
            if (auto socket = socketWeak_.lock()) {
                std::unique_lock<std::mutex> locker(mutexSocket_);

                auto handler = [locker_ {std::move(locker)} ](boost::system::error_code ec, std::size_t bytes) mutable {
                    locker_.unlock();

                    if (ec.failed()) {
                        log::write(log::Level::error, "SocketWriter",
                                   boost::format("failed send response: %1%") % ec.message());
                    } else {
                        log::write(log::Level::info, "SocketWriter",
                                   boost::format("send response: OK, count bytes = %1%") % bytes);
                    }
                };

                boost::asio::async_write(
                        *socket,
                        boost::asio::buffer(message),
                        std::move(handler));
            } else {
                log::write(log::Level::error, "SocketWriter", "writeImpl_ socket is close");
            }
        }
    }

    ClientSession::ClientSession(AsyncContextWeakPtr ctxWeak, engineWeakPtr engine, socket_t && socket, std::shared_ptr<ThreadSafeQueue> const& queue) :
        ctx_(std::move(ctxWeak)),
        engine_(std::move(engine)),
        socket_(std::make_shared<socket_t>(std::move(socket))),
        queue_(queue),
        writer_(std::make_shared<detail::SocketWriter>(queue_, detail::weak_from(socket_)))
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

        auto callback = [selfWeak = weak_from_this()](boost::system::error_code ec, std::size_t nbytes) mutable {
            if (auto self = selfWeak.lock()) {
                self->runRead(ec, nbytes);
            }
        };

        reenter(coroData_.coro_) for(;;)
        {
            log::write(log::Level::trace, "ClientSession", "read header");
            yield boost::asio::async_read(*socket_,
                                          boost::asio::buffer(coroData_.bufferHeader_.data(), detail::MAX_SIZE_HEADER_BUFFER),
                                          callback);

            log::write(log::Level::trace, "ClientSession", "read body");

            coroData_.header.ParseFromArray(coroData_.bufferHeader_.data(), detail::MAX_SIZE_HEADER_BUFFER);

            coroData_.bufferBody_.resize(coroData_.header.length());
            yield boost::asio::async_read(*socket_,
                                          boost::asio::buffer(coroData_.bufferBody_.data(), coroData_.header.length()),
                                          callback);
            coroData_.request.ParseFromArray(coroData_.bufferBody_.data(),
                                             static_cast<int>(coroData_.header.length()));

            AsyncContext::runAsync(ctx_, &ClientSession::processRequest, this, coroData_.header, coroData_.request);
        }
    }

    void ClientSession::write(std::vector<uint8_t> && message)
    {
        writer_->write(std::move(message));
    }

    void ClientSession::processRequest(Serialize::Header const& header, Serialize::Request const& request)
    {
        switch (static_cast<command::TypeCommand>(header.command()))
        {
            case command::TypeCommand::Unknown:
                log::write(log::Level::error, "processRequest", "Unknown command in header");
                break;
            case command::TypeCommand::RegistrationRequest:
                log::write(log::Level::info, "processRequest", "get Registration request");
                if (request.has_registration_request()) {
                    log::write(log::Level::debug, "processRequest", boost::format("login=%1%, password=%2%")
                                                                    % request.registration_request().login() % request.registration_request().password());
                    if (auto engine = engine_.lock()) {
                        engine->reg(weak_from_this(), request.registration_request());
                    }
                } else {
                    log::write(log::Level::error, "processRequest", "RegistrationRequest: Mismatch command in header and type request in body");
                    auto buffer = MsgFactory::serialize<command::TypeCommand::RegistrationResponse>(0);
                    write(std::move(buffer));
                }
                break;
            case command::TypeCommand::RegistrationResponse:
                log::write(log::Level::error, "processRequest", "RegistrationResponse should not come here");
                break;
            case command::TypeCommand::AuthorisationRequest:
                log::write(log::Level::info, "processRequest", "get Authorisation request");
                if (request.has_authorisation_request()) {
                    log::write(log::Level::debug, "processRequest", boost::format("login=%1%, password=%2%")
                        % request.authorisation_request().login() % request.authorisation_request().password());
                    if (auto engine = engine_.lock()) {
                        engine->auth(weak_from_this(), request.authorisation_request());
                    }
                } else {
                    log::write(log::Level::error, "processRequest", "AuthorisationRequest: Mismatch command in header and type request in body");
                    auto buffer = MsgFactory::serialize<command::TypeCommand::AuthorisationResponse>(0);
                    write(std::move(buffer));
                }
                break;
            case command::TypeCommand::AuthorisationResponse:
                log::write(log::Level::error, "processRequest", "AuthorisationResponse should not come here");
                break;
            case command::TypeCommand::SendTextRequest:
                log::write(log::Level::info, "processRequest", "SendText request");
                if (request.has_text_request()) {
                    log::write(log::Level::debug, "processRequest",
                               boost::format("login=%1%, channel_name=%2%, room_id=%3%. text=%4%")
                        % request.text_request().login() % request.text_request().channel_name() % request.text_request().room_id() % request.text_request().text());
                    if (auto engine = engine_.lock()) {
                        engine->sendText(request.text_request());
                    }
                } else {
                    log::write(log::Level::error, "processRequest", "SendTextRequest: Mismatch command in header and type request in body");
//                    auto buffer = MsgFactory::serialize<command::TypeCommand::SendTextRequest>("", false);
//                    write(buffer);
                }
                break;
            case command::TypeCommand::EchoResponse:
                log::write(log::Level::error, "processRequest", "EchoResponse should not come here");
                break;
            case command::TypeCommand::JoinRoomRequest:
                log::write(log::Level::info, "processRequest", "JoinRoom request");
                if(request.has_join_room_request()) {
                    log::write(log::Level::debug, "processRequest",
                               boost::format("client_id=%1%, channel_name=%2%")
                               % request.join_room_request().client_id() % request.join_room_request().channel_name());
                    if (auto engine = engine_.lock()) {
                        engine->joinRoom(request.join_room_request());
                    }
                } else {
                    log::write(log::Level::error, "processRequest", "JoinRoomRequest: Mismatch command in header and type request in body");
                    auto buffer = MsgFactory::serialize<command::TypeCommand::JoinRoomResponse>("", false);
                    write(std::move(buffer));
                }
                break;
            case command::TypeCommand::JoinRoomResponse:
                log::write(log::Level::error, "processRequest", "JoinRoomResponse should not come here");
                break;
            case command::TypeCommand::HistoryRequest: {
                log::write(log::Level::info, "processRequest", "History request");

                if (request.has_history_request()) {
                    if (auto engine = engine_.lock()) {
                        engine->getHistory(request.history_request());
                    }
                } else {
                    log::write(log::Level::error, "processRequest",
                               "HistoryRequest: Mismatch command in header and type request in body");
                }
                break;
            }
            case command::TypeCommand::HistoryResponse:
                log::write(log::Level::error, "processRequest", "HistoryResponse should not come here");
                break;
            case command::TypeCommand::ChannelsRequest:
                log::write(log::Level::info, "processRequest", "Channels request");
                if (request.has_channels_request()) {
                    if (auto engine = engine_.lock()) {
                        engine->getChannels(request.channels_request());
                    }
                } else {
                    log::write(log::Level::error, "processRequest",
                               "HistoryRequest: Mismatch command in header and type request in body");
                }
                break;
            case command::TypeCommand::ChannelsResponse:
                log::write(log::Level::error, "processRequest", "ChannelsResponse should not come here");
                break;
            default:
                log::write(log::Level::error, "processRequest", "Failed command in header");
                break;
        }
    }

}