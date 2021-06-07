//
// Created by focus on 06.06.2021.
//

#ifndef GOODOK_SERVERS_USER_H
#define GOODOK_SERVERS_USER_H

#include "IUser.h"

namespace goodok {
    class User : public IUser {
    public:
        User(sessionWeakPtr const& sessionWeak, std::string const& name, std::string const& password);

        void updateSession(sessionWeakPtr session) override { session_ = session; }
        sessionWeakPtr getSession() const override { return session_; }
        std::string getName() const override { return login_; }
        std::string getPassword() const override { return password_; }

        void setId(std::size_t id) override { id_ = id; }
        std::size_t getId() const override { return id_; }

        ~User();

    private:
        sessionWeakPtr session_;
        std::string login_;
        std::string password_;
        std::size_t id_ = 0;
    };
}


#endif //GOODOK_SERVERS_USER_H
