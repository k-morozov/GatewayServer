//
// Created by focus on 15.06.2021.
//

#include "ThreadSafeQueue.h"

namespace goodok {

ThreadSafeQueue::ThreadSafeQueue(std::function<void(buffer_t)> handler) :
    handlerTasks_(std::move(handler))
{

}

ThreadSafeQueue::~ThreadSafeQueue()
{
    quit_ = true;
    cv_.notify_all();
    for(auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}



void ThreadSafeQueue::start(std::size_t threadCount)
{
    for(std::size_t i=0; i<threadCount; ++i) {
        threads_.emplace_back(&ThreadSafeQueue::worker, this);
    }
    quit_ = false;
}

void ThreadSafeQueue::push(buffer_t message)
{
    std::lock_guard<std::mutex> g(cv_mutex_);
    tasks_.push(std::move(message));
    log::write(log::Level::info, "ThreadSafeQueue", "push task");
    notify();
}

void ThreadSafeQueue::push(std::function<void()> && task)
{
    std::lock_guard<std::mutex> g(cv_mutex_);
    queueTasks_.push(std::move(task));
    log::write(log::Level::info, "ThreadSafeQueue", "push task");
    notify();
}

void ThreadSafeQueue::worker()
{
    while(!quit_) {
        std::unique_lock<std::mutex> lock(cv_mutex_);
        cv_.wait(lock, [this]() {
            return !queueTasks_.empty() || quit_;
        });
        if (!queueTasks_.empty()) {
            auto task = std::move(queueTasks_.front());
            queueTasks_.pop();
            lock.unlock();

            try {
                task();
                log::write(log::Level::info, "ThreadSafeQueue", "successfully complete task");
            } catch (std::exception& ex) {
                log::write(log::Level::info, "ThreadSafeQueue", boost::format("failed complete task. exception = %1%") % ex.what());
            }


        }
    }

    std::lock_guard<std::mutex> g(cv_mutex_);
}

void ThreadSafeQueue::notify()
{
    cv_.notify_all();
}

}