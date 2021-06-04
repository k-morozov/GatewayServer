//
// Created by focus on 30.05.2021.
//

#ifndef GOODOK_FRONT_SERVER_ASYNCCONTEXT_H
#define GOODOK_FRONT_SERVER_ASYNCCONTEXT_H

#include "tools/log/Logger.h"
#include "WorkersPool.h"

#include <concepts>
#include <memory>

namespace goodok {

    class AsyncContext;
    using AsyncContextWeakPtr = std::weak_ptr<AsyncContext>;
    using AsyncContextSPtr = std::shared_ptr<AsyncContext>;

    template <class ... Args>
    concept ConceptIsInvokeArg = (
            std::is_invocable<Args...>::value
            );

    class AsyncContext {
    public:
        AsyncContext();
        ~AsyncContext();

        template<class Func, class ... Args,
                typename = std::enable_if<std::is_invocable<Func, Args...>::value>>
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
        template<ConceptIsInvokeArg Task>
        void runAsyncImpl(Task && task) const;
    };

    template<ConceptIsInvokeArg Task>
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

//    template<class Func, class ... Args>
//    void AsyncContext::runAsync(AsyncContextWeakPtr const& weakCtx, Func && func, Args &&... args)
//    {
//        if (auto ctx = weakCtx.lock()) {
//            ctx->runAsyncImpl(std::bind(
//                    std::forward<Func>(func),
//                    std::forward<Args>(args)...))
//            ;
//        }
//    }
}
#endif //GOODOK_FRONT_SERVER_ASYNCCONTEXT_H
