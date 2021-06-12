//
// Created by focus on 06.06.2021.
//

#include "User.h"

namespace goodok {

    User::User(UserSettings const& settings) :
        session_(settings.sessionWeak),
        login_(settings.name),
        password_(settings.password),
        id_(settings.id)
    {

    }

    User::~User() {

    }
}