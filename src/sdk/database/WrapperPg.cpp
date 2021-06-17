//
// Created by focus on 13.06.2021.
//

#include "WrapperPg.h"

#include "sdk/common/log/Logger.h"

#include <iomanip>

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

    PGresult * WrapperPg::execThreadSafe(std::string const& query) const
    {
        std::lock_guard g(mutex_);
        PGresult *res = PQexec(connection, query.c_str());;
        return res;
    }

    type_id_user WrapperPg::getClientId(std::string const& client_name) const
    {
        type_id_user client_id = REG_LOGIN_IS_BUSY;
        if (isConnected) {
            const std::string query = "SELECT id FROM clients WHERE login='" + client_name + "';";
            PGresult *res = execThreadSafe(query);
            if (PQresultStatus(res) != PGRES_TUPLES_OK) {
                log::write(log::Level::error, "WrapperPg", boost::format("getClientId: %1%") % PQresultErrorMessage(res));
            } else {
                client_id = PQntuples(res) ? std::stoi(PQgetvalue(res, 0, 0)) : 0;
            }
            PQclear(res);
        }

        return client_id;
    }

    type_id_user WrapperPg::getChannelId(std::string const& channel_name) const
    {
        type_id_user channel_id = LOGIN_IS_FREE;
        if (!isConnected) {
            log::write(log::Level::warning, "WrapperPg", "no connect to db");
            return channel_id;
        }

        const std::string query = "SELECT DISTINCT id FROM channels WHERE channel_name='" + channel_name + "';";
        PGresult *res = execThreadSafe(query);
        if (PQresultStatus(res) == PGRES_TUPLES_OK) {
            if (PQntuples(res)) {
                log::write(log::Level::info,  "WrapperPg", boost::format("channel_id = %1%") % PQgetvalue(res, 0, 0));
                channel_id = std::stoi(PQgetvalue(res, 0, 0));
                log::write(log::Level::info,  "WrapperPg", boost::format("channel_name=%1% has channel_id=%2%") % channel_name % channel_id);
            }
            PQclear(res);
        } else {
            log::write(log::Level::error, "WrapperPg", boost::format("getChannelId: channel_name=%1%, %2%")
                % channel_name % PQresultErrorMessage(res));
        }

        return channel_id;
    }

    std::string WrapperPg::getChannelName(type_id_user channel_id) const
    {
        std::string channel_name;
        if (isConnected) {
            const std::string query = "SELECT channel_name FROM channels WHERE id='" + std::to_string(channel_id) + "';";
            PGresult *res = execThreadSafe(query);
            if (PQresultStatus(res) != PGRES_TUPLES_OK) {
                log::write(log::Level::error, "WrapperPg", boost::format("getChannelName: %1%") % PQresultErrorMessage(res));
            } else {
                // @TODO many channels with EQ names?
                channel_name = PQntuples(res) ? PQgetvalue(res, 0, 0) : "";
            }
            PQclear(res);
        }

        return channel_name;
    }

    std::string WrapperPg::getClientName(type_id_user client_id) const
    {
        std::string client_name;
        if (!isConnected) {
            log::write(log::Level::warning, "WrapperPg", "no connect to db");
            return client_name;
        }

        const std::string query = "SELECT login FROM clients WHERE id='" + std::to_string(client_id) + "';";
        PGresult *res = execThreadSafe(query);
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            log::write(log::Level::error, "WrapperPg", boost::format("getClientName: %1%") % PQresultErrorMessage(res));
        } else {
            // @TODO many channels with EQ names?
            client_name = PQntuples(res) ? PQgetvalue(res, 0, 0) : "";
        }
        PQclear(res);


        return client_name;
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
            PGresult *res = execThreadSafe(query);
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
        PGresult *res = execThreadSafe(query);
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
        const std::string query = "SELECT DISTINCT channel_name FROM history WHERE client_id=" + std::to_string(client_id) + ";";
        PGresult *res = execThreadSafe(query);
        if (PQresultStatus(res) == PGRES_TUPLES_OK) {
            for(int i=0; i < PQntuples(res); ++i) {
                log::write(log::Level::debug, "WrapperPg", boost::format("channel: channel_name=%1%") % PQgetvalue(res, i, 0));
                channels.emplace_back(PQgetvalue(res, i, 0));
            }
        } else {
            log::write(log::Level::error, "WrapperPg", boost::format("getUserNameChannels: %1%") % PQresultErrorMessage(res));
        }
        PQclear(res);
        return channels;
    }

    std::vector<type_id_user> WrapperPg::getChannelUsers(type_id_user const& channel_id) const
    {
        std::vector<type_id_user> users;
        if (!isConnected) {
            log::write(log::Level::warning, "WrapperPg", "no connect to db");
            return users;
        }

        const std::string query = "SELECT DISTINCT client_id FROM subscriptions WHERE channel_id=" + std::to_string(channel_id) + ";";
        PGresult *res = execThreadSafe(query);
        if (PQresultStatus(res) == PGRES_TUPLES_OK) {
            log::write(log::Level::debug, "WrapperPg", boost::format("channel_id=%1% has users") % channel_id);
            for(int i=0; i < PQntuples(res); ++i) {
                log::write(log::Level::debug, "WrapperPg", boost::format("user has channel: %1%") % PQgetvalue(res, i, 0));
                users.emplace_back(std::stoi(PQgetvalue(res, i, 0)));
            }
        } else {
            log::write(log::Level::error, "WrapperPg", boost::format("getUserNameChannels: %1%") % PQresultErrorMessage(res));
        }
        PQclear(res);

        return users;
    }

    bool WrapperPg::hasChannel(std::string const& channelName) const {
        bool result = false;
        if (!isConnected) {
            log::write(log::Level::warning, "WrapperPg", "no connect to db");
            return result;
        }

        const std::string query = "SELECT channel_name FROM channels WHERE channel_name='" + channelName + "';";
        PGresult *res = execThreadSafe(query);
        if (PQresultStatus(res) == PGRES_TUPLES_OK) {
            result = PQntuples(res) > 0;
        } else {
            log::write(log::Level::error, "WrapperPg", boost::format("hasChannel: query=%1%. %2%") % query % PQresultErrorMessage(res));
            throw std::invalid_argument("wrong hasChannel");
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

        channel_id = getChannelId(channel_name);
        if (channel_id == LOGIN_IS_FREE) {
            log::write(log::Level::info, "WrapperPg",
                       boost::format("createChannel: db not contains channel_name=%1%. started generate id.") % channel_name);
            const std::string query =
                    "INSERT INTO channels(channel_name) VALUES ('" + channel_name + "');";
            PGresult *res = execThreadSafe(query);
            if (PQresultStatus(res) == PGRES_COMMAND_OK) {
                log::write(log::Level::info, "WrapperPg",
                           boost::format("createChannel: add channel_name=%1% to db.") % channel_name);
            } else {
                log::write(log::Level::error, "WrapperPg",
                           boost::format("createChannel: failed push channel_name=%1% to db. %2%") % channel_name % PQresultErrorMessage(res));
            }
            PQclear(res);

            channel_id = getChannelId(channel_name);
            if (channel_id == LOGIN_IS_FREE) {
                log::write(log::Level::error, "WrapperPg",
                           boost::format("createChannel: failed get login=%1% from db") % channel_name);
            }
        } else {
            log::write(log::Level::error, "WrapperPg",
                       boost::format("createChannel: channel_name=%1% is exist yet") % channel_name);
        }

        return channel_id;

    }

    bool WrapperPg::hasChannelClient(type_id_user channel_id, type_id_user client_id) const
    {
        bool result = false;
        if (!isConnected) {
            log::write(log::Level::warning, "WrapperPg", "no connect to db");
            return result;
        }
        const std::string query = "SELECT * FROM subscriptions WHERE client_id='"
                + std::to_string(client_id)
                + "' AND channel_id='"
                + std::to_string(channel_id) + "';";
        PGresult *res = execThreadSafe(query);
        if (PQresultStatus(res) == PGRES_TUPLES_OK) {
            result = PQntuples(res) > 0;
        } else {
            log::write(log::Level::error, "WrapperPg", boost::format("hasChannelClient: %1%") % PQresultErrorMessage(res));
        }
        PQclear(res);
        return result;
    }

    void WrapperPg::joinClientChannel(type_id_user client_id, std::string const& channel_name) {
        if (!isConnected) {
            log::write(log::Level::warning, "WrapperPg", "no connect to db");
            return;
        }
        type_id_user channel_id = getChannelId(channel_name);
        if (hasChannelClient(channel_id, client_id)) {
            return;
        }
        const std::string query = "INSERT INTO subscriptions VALUES(" + std::to_string(client_id) + ", " + std::to_string(channel_id) + ");";
        PGresult *res = execThreadSafe(query);
        if (PQresultStatus(res) == PGRES_COMMAND_OK) {
            log::write(log::Level::info, "WrapperPg", boost::format("joinClientChannel: successfully add client_id=%1% to channel=")
                % client_id % channel_name);
        } else {
            log::write(log::Level::error, "WrapperPg", boost::format("joinClientChannel: %1%") % PQresultErrorMessage(res));
        }
        PQclear(res);
    }

    void WrapperPg::addMsgHistory(type_id_user channel_id, command::ClientTextMsg && message) {
        PGresult *res;
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(4) << message.dt.date.year << "-"
           << std::setfill('0') << std::setw(2) << message.dt.date.month << "-"
           << std::setfill('0') << std::setw(2) << message.dt.date.day << " "
           << std::setfill('0') << std::setw(2) << message.dt.time.hours << ":"
           << std::setfill('0') << std::setw(2) << message.dt.time.minutes << ":"
           << std::setfill('0') << std::setw(2) << message.dt.time.seconds;

        const std::string query =
                "insert into history(client_id, channel_name, datetime, message) "
                "values (" + std::to_string(getClientId(message.author)) + ", '"
                + message.channel_name + "', '"
                + ss.str() + "', '"
                + message.text + "');";
        res = PQexec(connection, query.c_str());
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            log::write(log::Level::error, "WrapperPg", boost::format("addMsgHistory: %1%") % PQresultErrorMessage(res));
        } else {
            log::write(log::Level::info, "WrapperPg",
                       boost::format("addMsgHistory: successfully add msg to channel_name=%1% to db") % message.channel_name);
        }
        PQclear(res);
    }

    std::deque<command::ClientTextMsg> WrapperPg::getHistory(type_id_user channel_id) {
        std::deque<command::ClientTextMsg> history;

        if (!isConnected) {
            log::write(log::Level::warning, "WrapperPg", "no connect to db");
            return history;
        }

        const std::string channel_name = getChannelName(channel_id);
        const std::string query = "SELECT * FROM history WHERE channel_name='" + channel_name + "';";
        PGresult *res = execThreadSafe(query);
        if (PQresultStatus(res) == PGRES_TUPLES_OK) {
            for (int i = 0; i < PQntuples(res); i++) {
                command::ClientTextMsg msg;
                msg.author = getClientName(std::stoi(PQgetvalue(res, i, 0)));
                msg.channel_name = channel_name;
                std::string dt = PQgetvalue(res, i, 2);
                msg.text = PQgetvalue(res, i, 3);

                std::istringstream iss(dt);
                std::tm tm;
                iss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
                msg.dt = tm;

                BOOST_LOG_TRIVIAL(info) << "DB: author=" << msg.author << ", channel=" << msg.channel_name << ", "
                                        << msg.dt.date.year << "." << msg.dt.date.month << "." << msg.dt.date.day << " "
                                        << msg.dt.time.hours << ":" << msg.dt.time.minutes << ", text=" << msg.text;

                history.push_back(std::move(msg));
            }
        } else {
            log::write(log::Level::error, "WrapperPg", boost::format("getHistory: %1%") % PQresultErrorMessage(res));
        }
        PQclear(res);

        return history;
    }

    std::unordered_map<type_id_user, std::string> WrapperPg::getCurrentChannels() const
    {
        std::unordered_map<type_id_user, std::string> channels;
        if (!isConnected) {
            log::write(log::Level::warning, "WrapperPg", "no connect to db");
            return channels;
        }

        const std::string query = "SELECT * FROM channels;";
        PGresult *res = execThreadSafe(query);
        if (PQresultStatus(res) == PGRES_TUPLES_OK) {
            for (int i = 0; i < PQntuples(res); i++) {
                type_id_user channel_id = std::stoi(PQgetvalue(res, i, 0));
                std::string channel_name = PQgetvalue(res, i, 1);
                channels[channel_id] = std::move(channel_name);
            }
        } else {
            log::write(log::Level::error, "WrapperPg", boost::format("getCurrentChannels: %1%") % PQresultErrorMessage(res));
        }
        PQclear(res);
        return channels;
    }

}