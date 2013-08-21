// MUD Programming
// Ron Penton
// (C)2003
// ItemDatabase.h - The class that stores all of the items within the MUD
// 
// 

#ifndef SIMPLEMUDITEMDATABASE_H
#define SIMPLEMUDITEMDATABASE_H


#include <string>
#include <map>

#include "EntityDatabase.h"
#include "Item.h"

namespace SimpleMUD
{


// --------------------------------------------------------------------
//  A database for items
// --------------------------------------------------------------------
class ItemDatabase : public EntityDatabase<Item>
{
public:

    static bool Load();

};  // end class ItemDatabase



}   // end namespace SimpleMUD

#endif
