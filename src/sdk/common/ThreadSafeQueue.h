//
// Created by focus on 15.06.2021.
//

#ifndef GOODOK_SERVERS_THREADSAFEQUEUE_H
#define GOODOK_SERVERS_THREADSAFEQUEUE_H

#include "sdk/common/log/Logger.h"

#include <condition_variable>
#include <functional>
#include <future>
#include <thread>
#include <queue>


namespace goodok {

//    @TODO tamplate?
    class ThreadSafeQueue {
        using buffer_t = std::vector<uint8_t>;
    public:
        explicit ThreadSafeQueue(std::function<void(buffer_t)> handler);

        void start(std::size_t threadCount = 2);

        [[deprecated]] void push(buffer_t message);

        void push(std::function<void()> && task);

        void notify();

        ~ThreadSafeQueue();

    private:
        std::queue<std::function<void()>> queueTasks_;

        [[deprecated]] std::queue<buffer_t> tasks_;
        [[deprecated]] std::function<void(buffer_t)> handlerTasks_;

        std::mutex cv_mutex_;
        std::condition_variable cv_;
        std::atomic<bool> quit_;

        std::vector<std::thread> threads_;

    private:
        void worker();
    };

}

#endif //GOODOK_SERVERS_THREADSAFEQUEUE_H
