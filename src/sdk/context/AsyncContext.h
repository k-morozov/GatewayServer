//
// Created by focus on 30.05.2021.
//

#ifndef GOODOK_FRONT_SERVER_ASYNCCONTEXT_H
#define GOODOK_FRONT_SERVER_ASYNCCONTEXT_H

#include "tools/log/Logger.h"
#include "WorkersPool.h"

#include <memory>

namespace goodok {

    class AsyncContext {
    public:
        AsyncContext();
        ~AsyncContext();

        template<class Task>
        void runAsync(Task &&task);
    private:
        std::unique_ptr<WorkersPool> workers_;
    };

    template<class Task>
    void AsyncContext::runAsync(Task &&task)
    {
        if (!workers_) {
            log::write(log::Level::error,
                       "AsyncContext",
                       "workers_ not valid");
        }

        workers_->post(std::forward<Task>(task));
    }
}
#endif //GOODOK_FRONT_SERVER_ASYNCCONTEXT_H
