//
// Created by focus on 30.05.2021.
//

#ifndef GOODOK_FRONT_SERVER_ASYNCCONTEXT_H
#define GOODOK_FRONT_SERVER_ASYNCCONTEXT_H

#include "tools/log/Logger.h"
#include "WorkersPool.h"

#include <memory>

namespace goodok {

    class AsyncContext;
    using AsyncContextWeakPtr = std::weak_ptr<AsyncContext>;
    using AsyncContextSPtr = std::shared_ptr<AsyncContext>;


    class AsyncContext {
    public:
        AsyncContext();
        ~AsyncContext();

        template<class Task>
        void runAsync(Task &&task) const;

        template<class Function, class ... Args>
        static void runAsync(AsyncContextWeakPtr const& weakCtx, Function && func,  Args &&... args);
    private:
        mutable std::unique_ptr<WorkersPool> workers_;
    };

    template<class Task>
    void AsyncContext::runAsync(Task &&task) const
    {
        if (!workers_) {
            log::write(log::Level::error,
                       "AsyncContext",
                       "workers_ not valid");
        }

        workers_->post(std::forward<Task>(task));
    }

    template<class Function, class... Args>
    void AsyncContext::runAsync(AsyncContextWeakPtr const& weakCtx, Function &&func, Args &&... args)
    {
        if (auto ctx = weakCtx.lock()) {
            ctx->runAsync(std::bind(std::forward<Function>(func), std::forward<Args>(args)...));
        }
    }
}
#endif //GOODOK_FRONT_SERVER_ASYNCCONTEXT_H
