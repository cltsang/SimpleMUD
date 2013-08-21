// MUD Programming
// Ron Penton
// (C)2003
// RoomDatabase.h - The class that stores all of the rooms within the MUD
// 
// 

#ifndef SIMPLEMUDROOMDATABASE_H
#define SIMPLEMUDROOMDATABASE_H


#include <cmath>
#include <string>
#include <map>
#include <set>
#include <stdexcept>

#include "EntityDatabase.h"
#include "Room.h"
#include "DatabasePointer.h"

namespace SimpleMUD
{


class RoomDatabase : public EntityDatabaseVector<Room>
{
public:

    static void LoadTemplates();
    static void LoadData();

};  // end class RoomDatabase



}   // end namespace SimpleMUD

#endif
