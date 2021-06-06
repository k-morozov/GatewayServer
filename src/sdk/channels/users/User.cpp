//
// Created by focus on 06.06.2021.
//

#include "User.h"

namespace goodok {

    User::User(sessionWeakPtr const& sessionWeak, std::string const& name, std::string const& password) :
        session_(sessionWeak),
        login_(name),
        password_(password)
    {

    }

    User::~User() {

    }
}