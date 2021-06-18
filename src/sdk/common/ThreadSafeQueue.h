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

    class ThreadSafeQueue {
        using buffer_t = std::vector<uint8_t>;
    public:
        ThreadSafeQueue() = default;

        void start(std::size_t threadCount = 4);

        template<class Func, class ... Args, typename = typename std::enable_if<std::is_invocable<Func, Args...>::value>::type>
        void push(Func && f, Args && ... args);

        void notify();

        ~ThreadSafeQueue();

    private:
        std::queue<std::function<void()>> queueTasks_;

        std::mutex cv_mutex_;
        std::condition_variable cv_;
        std::atomic<bool> quit_;

        std::vector<std::thread> threads_;

    private:
        void worker();
    };

    template<class Func, class ... Args, class >
    void ThreadSafeQueue::push(Func && f, Args && ... args)
    {
        auto func = [f = std::forward<Func>(f), t = std::make_tuple(std::forward<Args>(args) ...)]() {
            std::apply(f, t);
        };
        std::lock_guard<std::mutex> g(cv_mutex_);
        queueTasks_.push(func);
        log::write(log::Level::info, "ThreadSafeQueue", "push task");
        notify();
    }
}


#endif //GOODOK_SERVERS_THREADSAFEQUEUE_H
