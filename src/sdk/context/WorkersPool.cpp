//
// Created by focus on 30.05.2021.
//

#include "WorkersPool.h"

namespace goodok {

    WorkersPool::WorkersPool(std::size_t countThread) :
            work_(std::make_unique<boost::asio::io_context::work>(io_context_))
    {
        for(std::size_t i=0; i<countThread; ++i)
        {
            pool_.emplace_back(std::thread(
                    [this]()
                    {
                        runThread();
                    }
                    ));
        }
        log::write(log::Level::info, "WorkersPool", "ctor done");
    }

    WorkersPool::~WorkersPool()
    {
        work_.reset();
        io_context_.stop();
        for(auto& th : pool_)
        {
            if (th.joinable()) {
                th.join();
            }
        }
        log::write(log::Level::info,
                   "WorkersPool",
                   boost::format("dtor done, counter tasks = %1%") % counter_.counter_);
    }

    void WorkersPool::runThread()
    {
//        log::write(log::Level::info,
//                   "WorkersPool",
//                   "run new thread");
        for(;;)
        {
            try
            {
                io_context_.run();
                break;
            }
            catch (std::exception &ex)
            {
                log::write(log::Level::error,
                           "WorkersPool",
                           ex.what());
            }

        } // end for
    }

} // end namespace goodok