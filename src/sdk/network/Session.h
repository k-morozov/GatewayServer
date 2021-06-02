//
// Created by focus on 31.05.2021.
//

#ifndef GOODOK_FRONT_SERVER_SESSION_H
#define GOODOK_FRONT_SERVER_SESSION_H

#include "tools/log/Logger.h"

#include "sdk/context/AsyncContext.h"

#include <boost/asio.hpp>
#include <boost/asio/coroutine.hpp>


namespace goodok {


    namespace Impl {
        struct CoroData {
            boost::asio::coroutine coro_;
            char bufferHeader_[3];
            char bufferBody_[3];
        };
    }

    class Session {
    public:
        Session(AsyncContextWeakPtr ctxWeak, boost::asio::ip::tcp::socket &&socket);

    private:
        AsyncContextWeakPtr ctx_;
        boost::asio::ip::tcp::socket socket_;

        Impl::CoroData coroData_;

    private:
        void runRead(boost::system::error_code = {}, std::size_t = 0);
    };

}

#endif //GOODOK_FRONT_SERVER_SESSION_H
