//
// Created by focus on 27.04.2021.
//

#include "gtest/gtest.h"

#include "sdk/context/AsyncContext.h"

#include <future>

using namespace goodok;

class TestContext : public ::testing::Test {
public:
    TestContext() {
        ctx = std::make_shared<goodok::AsyncContext>();
    }

    auto getWeakContext() {
        return std::weak_ptr<goodok::AsyncContext>(ctx);
    }

private:
    goodok::AsyncContextSPtr ctx;
};

TEST_F(TestContext, simpleOneTask) {
    bool status = false;
    auto promise = std::make_shared<std::promise<void>>();
    auto future = promise->get_future();

    auto task = [&status, promise]() {
        status = true;
        promise->set_value();
    };

    goodok::AsyncContext::runAsync(getWeakContext(), task);

    future.get();

    bool expected = true;
    ASSERT_EQ(status, expected);
}

TEST_F(TestContext, simpleSomeTaskLine) {
    std::atomic<int> status = 0;
    auto promise1 = std::make_shared<std::promise<void>>();
    auto future1 = promise1->get_future();
    auto task1 = [&status, promise1]() {
        ++status;
        promise1->set_value();
    };

    auto promise2 = std::make_shared<std::promise<void>>();
    auto future2 = promise2->get_future();
    auto task2 = [&status, promise2]() {
        ++status;
        promise2->set_value();
    };

    goodok::AsyncContext::runAsync(getWeakContext(), task1);
    goodok::AsyncContext::runAsync(getWeakContext(), task2);

    future1.get();
    future2.get();

    int expected = 2;

    ASSERT_EQ(status, expected);
}

TEST_F(TestContext, simpleTwoTaskFromPrev) {
    std::atomic<int> status = 0;
    auto promise = std::make_shared<std::promise<void>>();
    auto future = promise->get_future();
    auto task = [&status, promise, ctx{getWeakContext()}]() {
        ++status;

        auto task = [&status, promise]() {
            ++status;
            promise->set_value();
        };
        goodok::AsyncContext::runAsync(ctx, task);
    };

    goodok::AsyncContext::runAsync(getWeakContext(), task);

    future.get();

    int expected = 2;
    ASSERT_EQ(status, expected);
}
