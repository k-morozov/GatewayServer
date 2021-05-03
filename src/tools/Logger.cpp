//
// Created by focus on 03.05.2021.
//

#include "Logger.h"


namespace goodok {
namespace log {

    namespace logging = boost::log;
    namespace sinks = boost::log::sinks;
    namespace src = boost::log::sources;
    namespace expr = boost::log::expressions;
    namespace attrs = boost::log::attributes;
    namespace keywords = boost::log::keywords;

    void configure(boost::log::trivial::severity_level lvl)
    {
        logging::add_file_log
        (
                keywords::file_name = "log%3N.txt",
                keywords::rotation_size = 10 * 1024 * 1024,
                keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
                keywords::format = "%LineID%: [%TimeStamp%]<%Severity%>:\n%Message%"
        );
        logging::add_console_log(std::cout, boost::log::keywords::format =
                "%LineID%: [%TimeStamp%]<%Severity%>:\n%Message%"
                );
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= lvl);
        logging::add_common_attributes();
    }

    void write(boost::log::trivial::severity_level lvl, std::string const& text)
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