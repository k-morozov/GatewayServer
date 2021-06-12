//
// Created by focus on 12.06.2021.
//

#include "WrapperPg.h"
#include "log/Logger.h"

namespace goodok::db {

    bool WrapperPg::connect(ConnectSettings const& settings) {
        log::write(log::Level::info, "WrapperPg",
                   boost::format("start connect to %1%") % settings.host);
    }

}
