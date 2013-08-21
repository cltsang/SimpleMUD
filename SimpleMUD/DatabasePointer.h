// MUD Programming
// Ron Penton
// (C)2003
// DatabasePointer.h - A "smart" pointer class that looks up items in a database
// 
// 

#ifndef SIMPLEMUDDATABASEPOINTER_H
#define SIMPLEMUDDATABASEPOINTER_H

#include <iostream>
#include "Entity.h"

using std::ostream;
using std::istream;


// ======================================================
// This is the DATABASEPOINTER macro, which defines a 
// database pointer proxy class. Why macros? Because
// I've learned that templates + circular dependencies
// are a VERY BAD combination when dealing with simple
// one pass compilers, like C++.
// ======================================================
#define DATABASEPOINTER( pt, t )                        \
class t;                                                \
class pt                                                \
{                                                       \
public:                                                 \
    pt( entityid p_id = 0 )                             \
        : m_id( p_id ) {}                               \
                                                        \
    pt& operator=( entityid p_id )                      \
    {                                                   \
        m_id = p_id;                                    \
        return *this;                                   \
    }                                                   \
                                                        \
    operator entityid()                                 \
    {                                                   \
        return m_id;                                    \
    }                                                   \
                                                        \
    t& operator*();                                     \
    t* operator->();                                    \
    operator t*();                                      \
                                                        \
    entityid m_id;                                      \
};                                                      \
                                                        \
inline ostream& operator<<( ostream& s, const pt& p )   \
{                                                       \
    s << p.m_id;                                        \
    return s;                                           \
}                                                       \
                                                        \
inline istream& operator>>( istream& s, pt& p )         \
{                                                       \
    s >> p.m_id;                                        \
    return s;                                           \
}




namespace SimpleMUD
{

DATABASEPOINTER( player, Player )
DATABASEPOINTER( item, Item )
DATABASEPOINTER( room, Room )
DATABASEPOINTER( enemy, Enemy )
DATABASEPOINTER( enemytemplate, EnemyTemplate )
DATABASEPOINTER( store, Store )

} // end namespace SimpleMUD

#endif
