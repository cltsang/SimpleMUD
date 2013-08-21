// MUD Programming
// Ron Penton
// (C)2003
// DatabasePointer.cpp - A "smart" pointer class that looks up items in a database
// 
// 

#include "DatabasePointer.h"
#include "ItemDatabase.h"
#include "PlayerDatabase.h"
#include "RoomDatabase.h"
#include "EnemyDatabase.h"
#include "StoreDatabase.h"



// ======================================================
// This is the DATABASEPOINTERIMPL macro, which defines a 
// database pointer proxy class. Why macros? Because
// I've learned that templates + circular dependencies
// are a VERY BAD combination when dealing with simple
// one pass compilers, like C++.
// ======================================================
#define DATABASEPOINTERIMPL( pt, t, db )                \
t& pt::operator*()                                      \
{                                                       \
    return db::get( m_id );                             \
}                                                       \
                                                        \
t* pt::operator->()                                     \
{                                                       \
    return &( db::get( m_id ) );                        \
}                                                       \
                                                        \
pt::operator t*()                                       \
{                                                       \
    if( m_id == 0 )                                     \
        return 0;                                       \
    return &( db::get( m_id ) );                        \
}



namespace SimpleMUD
{

DATABASEPOINTERIMPL( player, Player, PlayerDatabase )
DATABASEPOINTERIMPL( item, Item, ItemDatabase )
DATABASEPOINTERIMPL( room, Room, RoomDatabase )
DATABASEPOINTERIMPL( enemy, Enemy, EnemyDatabase)
DATABASEPOINTERIMPL( enemytemplate, EnemyTemplate, EnemyTemplateDatabase )
DATABASEPOINTERIMPL( store, Store, StoreDatabase )

} // end namespace SimpleMUD
