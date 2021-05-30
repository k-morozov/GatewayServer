//
// Created by focus on 30.05.2021.
//

#ifndef GOODOK_FRONT_SERVER_WORKERSPOOL_H
#define GOODOK_FRONT_SERVER_WORKERSPOOL_H

#include "tools/log/Logger.h"

#include <boost/asio/io_context.hpp>

#include <thread>


namespace goodok {

    class WorkersPool;
    using WorkersPoolPtr = std::shared_ptr<WorkersPool>;

    struct CounterTasks {
        std::atomic<std::size_t> counter_ = 0;
        void startTask() { ++counter_; }
        void endTask() { --counter_; }
    };

    class WorkersPool {
    public:
        WorkersPool(std::size_t countThread = 4);
        ~WorkersPool();

        template<class Task>
        void post(Task &&task);

        auto &getContext() { return io_context_; }

    private:
        boost::asio::io_context io_context_;
        std::unique_ptr<boost::asio::io_context::work> work_;
        std::vector<std::thread> pool_;

        CounterTasks counter_;

        void runThread();
    };

    template<class Task>
    void WorkersPool::post(Task &&task)
    {
        counter_.startTask();
        io_context_.post(
            [this, Func{std::forward<Task>(task)}]() mutable
            {
                Func();
                counter_.endTask();
            });
    }

} // end namespace goodok

#endif //GOODOK_FRONT_SERVER_WORKERSPOOL_H
