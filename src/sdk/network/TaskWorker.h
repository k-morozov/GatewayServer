//
// Created by focus on 30.05.2021.
//

#ifndef GOODOK_FRONT_SERVER_TASKWORKER_H
#define GOODOK_FRONT_SERVER_TASKWORKER_H


#include <boost/asio/io_context.hpp>


class TaskWorker {
public:
    TaskWorker() = default;
    ~TaskWorker() = default;

    template<class Task>
    void post(Task && task);

    // @OTODO dispatch?

private:
    boost::asio::io_context io_context_;
    // @TODO threads?
};

template<class Task>
void TaskWorker::post(Task &&task)
{
    // @TODO post
}


#endif //GOODOK_FRONT_SERVER_TASKWORKER_H
