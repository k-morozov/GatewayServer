//
// Created by focus on 27.04.2021.
//

#include "gtest/gtest.h"
#include "sdk/common/log/Logger.h"

int main(int argc, char **argv) {
    goodok::log::configure(goodok::log::Level::fatal);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}