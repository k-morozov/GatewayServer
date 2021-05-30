//
// Created by focus on 02.05.2021.
//


#include <protocol/protocol.h>

#include "tools/log/Logger.h"
#include "sdk/context/WorkersPool.h"

#include <boost/program_options.hpp>


#include <iostream>
#include <thread>

constexpr const char* userOpt = "name";
constexpr const char* passOpt = "pass";
constexpr const char* helpOpt = "help";
constexpr const char* logLvl = "";

struct Params
{
    std::string username;
    std::string password;
};

Params setParameters(int argc, char** argv) {
    namespace po = boost::program_options;

    Params params;
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help, h", "produce help message")
                (userOpt, po::value<std::string>(), "username")
                (passOpt, po::value<std::string>(), "password");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count(userOpt) != 0U) {
            params.username = vm[userOpt].as<std::string>();
        }
        if (vm.count(passOpt) != 0U) {
            params.password = vm[passOpt].as<std::string>();
        }
    }
    catch(...)
    {
        std::cerr << "Failed parse params" << std::endl;
    }

    return params;
}

int main(int argc, char *argv[])
{
    auto params = setParameters(argc, argv);
    goodok::log::configure(goodok::log::Level::info);

    auto task = []()
    {
        static int value = 0;
        goodok::log::write(goodok::log::Level::info,
                           "TaskLambda",
                           boost::format("current counter = %1%") % value);
        ++value;
    };

    {
        goodok::WorkersPoolPtr worker = std::make_shared<goodok::WorkersPool>();

        worker->post(task);
        worker->post(task);
        worker->post(task);
    }
}