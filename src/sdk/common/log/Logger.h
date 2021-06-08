//
// Created by focus on 03.05.2021.
//

#ifndef GOODOK_FRONT_SERVER_LOGGER_H
#define GOODOK_FRONT_SERVER_LOGGER_H

#include <boost/format.hpp>

#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/exceptions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace goodok {
namespace log {

    using Level = boost::log::trivial::severity_level;

    void configure(boost::log::trivial::severity_level lvl = boost::log::trivial::severity_level::trace);

    void write(boost::log::trivial::severity_level lvl, std::string const& location, std::string const& text);

    void write(boost::log::trivial::severity_level lvl, std::string const& location, boost::format const& text);

    void write(boost::log::trivial::severity_level lvl, boost::format const& location, boost::format const& text);

    void write(boost::log::trivial::severity_level lvl, boost::format const& location,std::string const& text);


} // end namespace log
} // end namespace goodok

#endif //GOODOK_FRONT_SERVER_LOGGER_H
