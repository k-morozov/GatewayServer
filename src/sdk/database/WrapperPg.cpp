//
// Created by focus on 12.06.2021.
//

#include "WrapperPg.h"
#include "log/Logger.h"

namespace goodok::db {

    bool WrapperPg::connect(ConnectSettings const& settings) {
        log::write(log::Level::info, "WrapperPg",
                   boost::format("start connect to %1%") % settings.host);
        return true;
    }

    type_id_user WrapperPg::checkRegUser(std::string const& clientName)
    {
        type_id_user id = REG_LOGIN_IS_BUSY;
        if (!nameUsers_.contains(clientName)) {
            id = generator_.generate();
        }
        return id;
    }

}
