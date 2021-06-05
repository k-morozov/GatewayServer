//
// Created by focus on 05.06.2021.
//

#ifndef GOODOK_SERVERS_MAKESHAREDHELPER_H
#define GOODOK_SERVERS_MAKESHAREDHELPER_H

template <class C>
struct MakeSharedHelper final: public C
{
    template <class ... Types>
    explicit MakeSharedHelper<C>(Types && ... args) : C(std::forward<Types>(args)...) {}
};


#endif //GOODOK_SERVERS_MAKESHAREDHELPER_H
