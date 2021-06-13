//
// Created by focus on 13.06.2021.
//

#include "WrapperPg.h"

#include "sdk/common/log/Logger.h"

namespace goodok::db {

    bool WrapperPg::connect(ConnectSettings const& setting)
    {
        if (!isConnected) {
            connection = PQconnectdb(
                    std::string("user=" + setting.user
                                + " password=" + setting.password
                                + " host=" + setting.host
                                + " dbname=" + setting.db
                    ).c_str()
                    );
        }
        if (PQstatus(connection) != CONNECTION_OK) {
            log::write(log::Level::error, "WrapperPg", "connect failed");
            PQerrorMessage(connection);
        } else {
            isConnected = true;
        }

        return isConnected;
    }

    type_id_user WrapperPg::getClientId(std::string const& client_name) const
    {
        type_id_user client_id = REG_LOGIN_IS_BUSY;
        if (isConnected) {
            const std::string query = "SELECT id FROM clients WHERE login='" + client_name + "';";
            PGresult *res = PQexec(connection, query.c_str());
            if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            } else {
                client_id = PQntuples(res) ? std::stoi(PQgetvalue(res, 0, 0)) : 0;
            }
            PQclear(res);
        }

        return client_id;
    }


    type_id_user WrapperPg::checkRegUser(InputSettings const& settings) {
        type_id_user client_id = LOGIN_IS_FREE;
        if (!isConnected) {
            log::write(log::Level::warning, "WrapperPg", "no connect to db");
            return client_id;
        }

        if (getClientId(settings.clientName) == LOGIN_IS_FREE) {
            const std::string query =
                    "INSERT INTO clients(login, password) VALUES ('" + settings.clientName + "', '" + settings.clientPassword +"');";
            PGresult * res = PQexec(connection, query.c_str());
            if (PQresultStatus(res) == PGRES_COMMAND_OK) {
                log::write(log::Level::info, "WrapperPg",
                           boost::format("checkRegUser: add login=%1% to db.") % settings.clientName);
            } else {
                log::write(log::Level::error, "WrapperPg",
                           boost::format("checkRegUser: failed push login=%1% to db. %2%") % settings.clientName % PQresultErrorMessage(res));
            }
            PQclear(res);
            client_id = getClientId(settings.clientName);
            if (client_id == LOGIN_IS_FREE) {
                log::write(log::Level::error, "WrapperPg",
                           boost::format("checkRegUser: failed get login=%1% from db") % settings.clientName);
            }
        } else {
            log::write(log::Level::error, "WrapperPg",
                       boost::format("checkRegUser: login=%1% is busy yet") % settings.clientName);
        }

        return client_id;
    }

    type_id_user WrapperPg::checkAuthUser(InputSettings const& settings) {
        type_id_user client_id = AUTH_LOGIN_IS_NOT_AVAILABLE;
        if (!isConnected) {
            log::write(log::Level::warning, "WrapperPg", "no connect to db");
            return client_id;
        }

        const std::string query = "SELECT id, password FROM clients WHERE login='" + settings.clientName + "';";
        PGresult *res = PQexec(connection, query.c_str());
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            log::write(log::Level::error, "WrapperPg",
                       boost::format("checkAuthUser: login=%1% not found in db") % settings.clientName);
        } else {
            if (PQntuples(res)) {
                if (settings.clientPassword == PQgetvalue(res, 0, 1)) {
                    client_id = std::stoi(PQgetvalue(res, 0, 0));
                    log::write(log::Level::info, "WrapperPg",
                               boost::format("checkAuthUser: successful find id = %1% for login=%2% in db")
                               % client_id % settings.clientName);
                } else {
                    log::write(log::Level::info, "WrapperPg",
                               boost::format("checkAuthUser: login=%1% incorrect password") % settings.clientName);
                }

            } else {
                client_id = AUTH_LOGIN_IS_NOT_AVAILABLE;
            }
        }
        PQclear(res);
        return client_id;
    }

    std::deque<std::string> WrapperPg::getUserNameChannels(type_id_user const& client_id) {
        std::deque<std::string> channels;
        if (!isConnected) {
            log::write(log::Level::warning, "WrapperPg", "no connect to db");
            return channels;
        }
        const std::string query = "SELECT channel_name FROM history WHERE client_id=" + std::to_string(client_id) + ";";
        PGresult *res = PQexec(connection, query.c_str());
        if (PQresultStatus(res) == PGRES_TUPLES_OK) {
            for(int i=0; i < PQntuples(res); ++i) {
                channels.emplace_back(PQgetvalue(res, i, 0));
            }
        } else {
            log::write(log::Level::error, "WrapperPg", boost::format("getUserNameChannels: %1%") % PQresultErrorMessage(res));
        }
        PQclear(res);
        return channels;
    }

    bool WrapperPg::hasChannel(std::string const& channelName) const {
        bool result = false;
        if (!isConnected) {
            log::write(log::Level::warning, "WrapperPg", "no connect to db");
            return result;
        }

        const std::string query = "SELECT channel_name FROM history WHERE channel_name=" + channelName + ";";
        PGresult *res = PQexec(connection, query.c_str());
        if (PQresultStatus(res) == PGRES_TUPLES_OK) {
            result = PQntuples(res) > 0;
        } else {
            log::write(log::Level::error, "WrapperPg", boost::format("hasChannel: %1%") % PQresultErrorMessage(res));
        }
        PQclear(res);

        return result;
    }

    type_id_user WrapperPg::createChannel(std::string const& channel_name) {
        type_id_user channel_id = LOGIN_IS_FREE;
        if (!isConnected) {
            log::write(log::Level::warning, "WrapperPg", "no connect to db");
            return channel_id;
        }

//        if (getClientId(settings.clientName) == LOGIN_IS_FREE) {
//            const std::string query =
//                    "INSERT INTO clients(login, password) VALUES ('" + settings.clientName + "', '" + settings.clientPassword +"');";
//            PGresult * res = PQexec(connection, query.c_str());
//            if (PQresultStatus(res) == PGRES_COMMAND_OK) {
//                log::write(log::Level::info, "WrapperPg",
//                           boost::format("checkRegUser: add login=%1% to db.") % settings.clientName);
//            } else {
//                log::write(log::Level::error, "WrapperPg",
//                           boost::format("checkRegUser: failed push login=%1% to db. %2%") % settings.clientName % PQresultErrorMessage(res));
//            }
//            PQclear(res);
//            client_id = getClientId(settings.clientName);
//            if (client_id == LOGIN_IS_FREE) {
//                log::write(log::Level::error, "WrapperPg",
//                           boost::format("checkRegUser: failed get login=%1% from db") % settings.clientName);
//            }
//        } else {
//            log::write(log::Level::error, "WrapperPg",
//                       boost::format("checkRegUser: login=%1% is busy yet") % settings.clientName);
//        }
//
//        return client_id;

    }

    void WrapperPg::joinClientChannel(type_id_user, std::string const&) {}

    void WrapperPg::addMsgHistory(type_id_user, command::ClientTextMsg const&) {}

    std::deque<command::ClientTextMsg> WrapperPg::getHistory(type_id_user) {}

}