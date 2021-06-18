//
// Created by focus on 30.05.2021.
//

#ifndef GOODOK_FRONT_SERVER_ASYNCCONTEXT_H
#define GOODOK_FRONT_SERVER_ASYNCCONTEXT_H

#include "sdk/common/log/Logger.h"
#include "WorkersPool.h"

#include <concepts>
#include <memory>

namespace goodok {

    class AsyncContext;
    using AsyncContextWeakPtr = std::weak_ptr<AsyncContext>;
    using AsyncContextSPtr = std::shared_ptr<AsyncContext>;

    template <class Func>
    concept IsInvoke = (
            std::is_invocable<Func>::value
            );

    class AsyncContext {
    public:
        AsyncContext();
        ~AsyncContext();

        template<class Func, class ... Args,
                typename = typename std::enable_if<std::is_invocable<Func, Args...>::value>::type>
        static void runAsync(AsyncContextWeakPtr const& weakCtx, Func && func, Args &&... args)
        {
            if (auto ctx = weakCtx.lock())
            {
                ctx->runAsyncImpl(std::bind(
                        std::forward<Func>(func),
                        std::forward<Args>(args)...));
            }
        }

    private:
        mutable std::unique_ptr<WorkersPool> workers_;

    private:
        template<IsInvoke Task>
        void runAsyncImpl(Task && task) const;
    };

    template<IsInvoke Task>
    void AsyncContext::runAsyncImpl(Task && task) const
    {
        if (!workers_) {
            log::write(log::Level::error,
                       "AsyncContext",
                       "workers_ not valid");
            return;
        }

        workers_->post(std::forward<Task>(task));
    }
}
#endif //GOODOK_FRONT_SERVER_ASYNCCONTEXT_H
