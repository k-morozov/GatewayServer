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


namespace goodok {

    namespace Impl {
        struct CoroData {
            boost::asio::coroutine coro_;
            boost::asio::coroutine coroWrite_;
            char bufferHeader_[3];
            char bufferBody_[3];
            std::list<std::string> bufferWrite_;
            std::atomic<bool> isSendNow = false;
        };
    }

    class ClientSession : public ISession {
    public:
        ClientSession(AsyncContextWeakPtr ctxWeak, boost::asio::ip::tcp::socket &&socket);
        ~ClientSession() override;

        void startRead() override;
        void write(std::string) override;

    private:
        AsyncContextWeakPtr ctx_;
        boost::asio::ip::tcp::socket socket_;

        Impl::CoroData coroData_;

    private:
        void runRead(boost::system::error_code = {}, std::size_t = 0);
        void writeImpl_(std::string const& message);
    };

}

#endif //GOODOK_FRONT_SERVER_SESSION_H
