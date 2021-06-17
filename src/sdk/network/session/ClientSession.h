//
// Created by focus on 31.05.2021.
//

#ifndef GOODOK_FRONT_SERVER_SESSION_H
#define GOODOK_FRONT_SERVER_SESSION_H

#include "ISession.h"

#include "log/Logger.h"
#include "sdk/context/AsyncContext.h"
#include "sdk/common/ThreadSafeQueue.h"
#include "sdk/engine/QueryEngine.h"

#include <boost/asio.hpp>
#include <boost/asio/coroutine.hpp>

#include <memory>
#include <execution>

namespace goodok {

    namespace detail {
        constexpr std::size_t MAX_SIZE_HEADER_BUFFER = goodok::SIZE_HEADER;
        constexpr std::size_t MAX_SIZE_BODY_BUFFER = 3;

        using buffer_t = std::vector<uint8_t>;
        using buffer_header_t = std::array<uint8_t, MAX_SIZE_HEADER_BUFFER>;
        using buffer_body_t = std::vector<uint8_t>;

        template <class T>
        std::weak_ptr<T> weak_from(std::shared_ptr<T> const& src) {
            return src;
        }

        [[deprecated]]
        inline buffer_t convert(std::string const& message) {
            buffer_t data(message.size());
            std::copy(std::execution::par,
                      std::begin(message), std::end(message),
                      std::begin(data));
            return data;
        }

        [[deprecated]]
        inline std::string convert(buffer_header_t const& message) {
            std::string data;
            std::copy(std::execution::par,
                      std::begin(message), std::end(message),
                      std::back_inserter(data));
            return data;
        }

        [[deprecated]]
        inline std::string convert(buffer_body_t const& message) {
            std::string data;
            std::copy(std::execution::par,
                      std::begin(message), std::end(message),
                      std::back_inserter(data));
            return data;
        }

        class SocketWriter : public std::enable_shared_from_this<SocketWriter>
        {
            using socket_t = boost::asio::ip::tcp::socket;
        public:
            explicit SocketWriter(std::weak_ptr<ThreadSafeQueue> queue, std::weak_ptr<socket_t> sock);
            ~SocketWriter() = default;

            void write(std::vector<uint8_t> &&);
        private:
            std::weak_ptr<socket_t> socketWeak_;
            mutable std::mutex mutexSocket_;
            std::weak_ptr<ThreadSafeQueue> queue_;

        private:
            void writeImpl_(std::vector<uint8_t> && message);
        };
    }

    class ClientSession :
            public std::enable_shared_from_this<ClientSession>,
            public ISession
    {
        using socket_t = boost::asio::ip::tcp::socket;
    public:

        ~ClientSession() override;

        void startRead() override;
        void write(std::vector<uint8_t> &&) override;

    protected:
        ClientSession(AsyncContextWeakPtr ctxWeak, engineWeakPtr engine, socket_t &&socket, std::shared_ptr<ThreadSafeQueue> const& queue);

    private:
        AsyncContextWeakPtr ctx_;
        engineWeakPtr engine_;
        std::shared_ptr<socket_t> socket_;
        std::weak_ptr<ThreadSafeQueue> queue_;
        std::shared_ptr<detail::SocketWriter> writer_;

        struct CoroData {
            boost::asio::coroutine coro_;
            detail::buffer_header_t bufferHeader_;
            detail::buffer_body_t bufferBody_;
            Serialize::Header header;
            Serialize::Request request;
        };
        CoroData coroData_;

    private:
        void runRead(boost::system::error_code = {}, std::size_t = 0);

        void processRequest(Serialize::Header const& header, Serialize::Request const& request);
    };

}

#endif //GOODOK_FRONT_SERVER_SESSION_H
