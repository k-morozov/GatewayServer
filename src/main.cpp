//
// Created by focus on 02.05.2021.
//


#include <protocol/protocol.h>

#include "tools/log/Logger.h"
#include "sdk/context/AsyncContext.h"
#include "sdk/network/Network.h"

#include <boost/program_options.hpp>


#include <iostream>

constexpr const char* userOpt = "name";
constexpr const char* passOpt = "pass";

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
    goodok::log::configure(goodok::log::Level::debug);

    auto ctx = std::make_shared<goodok::AsyncContext>();
    auto nwk = std::make_shared<goodok::Network>(ctx, 7777);

    nwk->run();
}