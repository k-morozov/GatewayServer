//
// Created by focus on 31.05.2021.
//

#ifndef GOODOK_FRONT_SERVER_NETWORK_H
#define GOODOK_FRONT_SERVER_NETWORK_H

#include "sdk/context/AsyncContext.h"
#include "Session.h"

#include "tools/log/Logger.h"

#include <boost/asio.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/asio/yield.hpp>


namespace goodok {

    template<typename SessionType>
    class AcceptProcess {
        using io_context = boost::asio::io_context;
        using tcp = boost::asio::ip::tcp;
    public:
        explicit AcceptProcess(AsyncContextWeakPtr ctx, int port = 7777);
        ~AcceptProcess();

        void run();

    private:
        AsyncContextWeakPtr ctx_;
        int port_;

        io_context networkContext_;
        boost::asio::coroutine coroAccept_;

        std::vector<std::shared_ptr<SessionType>> sessions_;

        tcp::endpoint endpoint_;
        tcp::acceptor acceptor_;
        tcp::socket socket_;

    private:
        void runAccept();
        void doAccept(boost::system::error_code = {});
    };

    // realisation


    template <typename T>
    AcceptProcess<T>::AcceptProcess(AsyncContextWeakPtr ctxWeak, int port) :
            ctx_(std::move(ctxWeak)),
            port_(port),
            endpoint_(tcp::v4(), port_),
            acceptor_(networkContext_, endpoint_),
            socket_(networkContext_)
    {
        runAccept();
        log::write(log::Level::debug, "Network", "ctor done");
    }

    template <typename T>
    AcceptProcess<T>::~AcceptProcess()
    {
        networkContext_.stop();
        if (socket_.is_open())
        {
            socket_.close();
        }
        log::write(log::Level::debug, "Network", "dtor done");
    }

    template <typename T>
    void AcceptProcess<T>::run()
    {
        networkContext_.run();
    }

    template <typename T>
    void AcceptProcess<T>::runAccept()
    {
        doAccept();
    }

    template <typename T>
    void AcceptProcess<T>::doAccept(boost::system::error_code)
    {
        // @TODO error code checker?
        reenter(coroAccept_) for(;;)
        {
            log::write(log::Level::trace, "Network", "wait accept...");
            yield acceptor_.async_accept(socket_,
                                         std::bind(&AcceptProcess::doAccept, this, std::placeholders::_1));
            log::write(log::Level::debug, "Network", "new connection");
            sessions_.emplace_back(std::make_shared<T>(ctx_, std::move(socket_)));
            sessions_.back()->start();
            socket_.close();
        }
    }

} // end namespace goodok

#endif //GOODOK_FRONT_SERVER_NETWORK_H
