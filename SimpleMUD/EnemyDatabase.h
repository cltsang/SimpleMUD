// MUD Programming
// Ron Penton
// (C)2003
// EnemyDatabase.h - The class that stores all of the enemies within the MUD
// 
// 

#ifndef ENEMYDATABASE_H
#define ENEMYDATABASE_H


#include "EntityDatabase.h"
#include "Enemy.h"
#include "DatabasePointer.h"

namespace SimpleMUD
{


class EnemyTemplateDatabase : public EntityDatabaseVector<EnemyTemplate>
{
public:
    static void Load();
};  // end class EnemyTemplateDatabase


class EnemyDatabase : public EntityDatabase<Enemy>
{
public:
    static void Create( entityid p_template, room p_room );
    static void Delete( enemy p_enemy );
    static void Load();

};  // end class EnemyDatabase



}   // end namespace SimpleMUD

#endif
