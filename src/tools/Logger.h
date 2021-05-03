//
// Created by focus on 03.05.2021.
//

#ifndef GOODOK_FRONT_SERVER_LOGGER_H
#define GOODOK_FRONT_SERVER_LOGGER_H

#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/exceptions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace goodok {
namespace log {

    namespace logging = boost::log;
    namespace sinks = boost::log::sinks;
    namespace src = boost::log::sources;
    namespace expr = boost::log::expressions;
    namespace attrs = boost::log::attributes;
    namespace keywords = boost::log::keywords;

    class Logger
    {
        using severity_level = boost::log::trivial::severity_level;
    public:
        Logger(severity_level lvl = severity_level::trace);
        ~Logger() = default;
    };

    inline void write(boost::log::trivial::severity_level lvl, std::string const& text)
    {
        using level = boost::log::trivial::severity_level;

        switch (lvl) {
            case level::trace:
                BOOST_LOG_TRIVIAL(trace) << text;
                break;
            case level::debug:
                BOOST_LOG_TRIVIAL(debug) << text;
                break;
            case level::info:
                BOOST_LOG_TRIVIAL(info) << text;
                break;
            case level::warning:
                BOOST_LOG_TRIVIAL(warning) << text;
                break;
            case level::error:
                BOOST_LOG_TRIVIAL(error) << text;
                break;
            case level::fatal :
                BOOST_LOG_TRIVIAL(fatal) << text;
                break;
            default:
                break;
        }
    }


} // end namespace log
} // end namespace goodok

#endif //GOODOK_FRONT_SERVER_LOGGER_H
