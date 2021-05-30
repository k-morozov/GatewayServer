//
// Created by focus on 30.05.2021.
//

#include "AsyncContext.h"

namespace goodok {

    AsyncContext::AsyncContext() :
        workers_(std::make_unique<WorkersPool>(2))
    {
        log::write(log::Level::info,
                   "AsyncContext",
                   "ctor done");
    }

    AsyncContext::~AsyncContext()
    {
        log::write(log::Level::info,
                   "AsyncContext",
                   "dtor done");
    }
}
