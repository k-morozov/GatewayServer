//
// Created by focus on 03.05.2021.
//

#include "Logger.h"


namespace goodok {
namespace log {

    Logger::Logger(severity_level lvl)
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

} // end namespace log
} // end namespace goodok