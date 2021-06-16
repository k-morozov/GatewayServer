//
// Created by focus on 31.05.2021.
//

#ifndef GOODOK_FRONT_SERVER_NETWORK_H
#define GOODOK_FRONT_SERVER_NETWORK_H

#include "sdk/common/MakeSharedHelper.h"
#include "sdk/common/log/Logger.h"
#include "sdk/context/AsyncContext.h"
#include "sdk/network/session/ClientSession.h"
#include "sdk/engine/QueryEngine.h"

#include <boost/asio.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/asio/yield.hpp>

#include <concepts>

namespace goodok {

    template <class T>
    concept ConceptSessionType = (std::derived_from<T, goodok::ISession>);

    template<ConceptSessionType SessionType>
    class AcceptProcess {
        using io_context = boost::asio::io_context;
        using tcp = boost::asio::ip::tcp;
    public:
        ~AcceptProcess();

    protected:
        explicit AcceptProcess(AsyncContextWeakPtr ctx, engineWeakPtr engine, int port, std::shared_ptr<ThreadSafeQueue> queue);

    public:
        void run();

    private:
        AsyncContextWeakPtr ctx_;
        engineWeakPtr engine_;
        int port_;
        std::shared_ptr<ThreadSafeQueue> queue_;

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


    template <ConceptSessionType T>
    AcceptProcess<T>::AcceptProcess(AsyncContextWeakPtr ctxWeak, engineWeakPtr engine, int port, std::shared_ptr<ThreadSafeQueue> queue) :
            ctx_(std::move(ctxWeak)),
            engine_(std::move(engine)),
            port_(port),
            endpoint_(tcp::v4(), port_),
            acceptor_(networkContext_, endpoint_),
            socket_(networkContext_),
            queue_(std::move(queue))
    {
        runAccept();
        log::write(log::Level::debug, "Network", "ctor done");
    }

    template <ConceptSessionType T>
    AcceptProcess<T>::~AcceptProcess()
    {
        networkContext_.stop();
        if (socket_.is_open())
        {
            socket_.close();
        }
        log::write(log::Level::debug, "Network", "dtor done");
    }

    template <ConceptSessionType T>
    void AcceptProcess<T>::run()
    {
        networkContext_.run();
    }

    template <ConceptSessionType T>
    void AcceptProcess<T>::runAccept()
    {
        doAccept();
    }

    template <ConceptSessionType T>
    void AcceptProcess<T>::doAccept(boost::system::error_code)
    {
        // @TODO error code checker?
        reenter(coroAccept_) for(;;)
        {
            log::write(log::Level::trace, "Network", "wait accept...");
            yield acceptor_.async_accept(socket_,
                                         std::bind(&AcceptProcess::doAccept, this, std::placeholders::_1));
            log::write(log::Level::debug, "Network", "new connection");
            auto session = std::make_shared<MakeSharedHelper<T>>(ctx_, engine_, std::move(socket_), queue_);
            sessions_.emplace_back(session);
            sessions_.back()->startRead();
            socket_.close();
        }
    }

} // end namespace goodok

#endif //GOODOK_FRONT_SERVER_NETWORK_H
