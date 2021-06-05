//
// Created by focus on 31.05.2021.
//

#ifndef GOODOK_FRONT_SERVER_SESSION_H
#define GOODOK_FRONT_SERVER_SESSION_H

#include "tools/log/Logger.h"

#include "sdk/context/AsyncContext.h"
#include "ISession.h"

#include <boost/asio.hpp>
#include <boost/asio/coroutine.hpp>

#include <memory>
#include <execution>

namespace goodok {

    namespace detail {
        using buffer_t = std::vector<uint8_t>;

        template <class T>
        std::weak_ptr<T> weak_from(std::shared_ptr<T> src) {
            return src;
        }

        inline buffer_t convert(std::string const& message) {
            buffer_t data(message.size());
            std::copy(std::execution::par,
                      std::begin(message), std::end(message),
                      std::begin(data));
//            log::write(log::Level::info, "convert", boost::format("size convert = %1%") % data.size());
            return data;
        }

        class SocketWriter : public std::enable_shared_from_this<SocketWriter>
        {
            using socket_t = boost::asio::ip::tcp::socket;
        public:
            explicit SocketWriter(std::weak_ptr<socket_t> sock);
            ~SocketWriter() = default;

            void write(std::string);
        private:
            std::weak_ptr<socket_t> socketWeak_;
            std::list<buffer_t> bufferWrite_;
            std::atomic<bool> processWrite = false;

        private:
            void writeImpl_();
        };
    }

    class ClientSession :
            public std::enable_shared_from_this<ClientSession>,
            public ISession
    {
        using socket_t = boost::asio::ip::tcp::socket;
    public:
        ClientSession(AsyncContextWeakPtr ctxWeak, socket_t &&socket);
        ~ClientSession() override;

        void startRead() override;
        void write(std::string) override;

    private:
        AsyncContextWeakPtr ctx_;
        std::shared_ptr<socket_t> socket_;
        std::shared_ptr<detail::SocketWriter> writer_;

        struct CoroData {
            boost::asio::coroutine coro_;
            char bufferHeader_[3];
            char bufferBody_[3];

        };
        CoroData coroData_;

    private:
        void runRead(boost::system::error_code = {}, std::size_t = 0);
    };

}

#endif //GOODOK_FRONT_SERVER_SESSION_H
