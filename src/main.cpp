//
// Created by focus on 02.05.2021.
//

#include <iostream>
#include <protocol/protocol.h>
#include "tools/Logger.h"

#include <boost/program_options.hpp>

constexpr const char* userOpt = "name";
constexpr const char* passOpt = "pass";
constexpr const char* helpOpt = "help";
constexpr const char* logLvl = "";

struct Params
{
    std::string username;
    std::string password;
    std::string sessionFile;
};

Params setParameters(int argc, char** argv) {
    namespace po = boost::program_options;

    Params params;
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help, h", "produce help message")
                (userOpt, po::value<std::string>(), "username for instagram account")
                (passOpt, po::value<std::string>(), "password for instagram account");

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
    goodok::log::configure(boost::log::trivial::severity_level::error);
    goodok::log::write(boost::log::trivial::severity_level::info, "Test logger");
}