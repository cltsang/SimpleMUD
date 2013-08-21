// MUD Programming
// Ron Penton
// (C)2003
// StoreDatabase.h - The class that stores all of the stores within the MUD
// 
// 

#ifndef SIMPLEMUDSTOREDATABASE_H
#define SIMPLEMUDSTOREDATABASE_H


#include <string>
#include <map>

#include "EntityDatabase.h"
#include "Store.h"

namespace SimpleMUD
{


// --------------------------------------------------------------------
//  A database for items
// --------------------------------------------------------------------
class StoreDatabase : public EntityDatabase<Store>
{
public:
    static bool Load();

};  // end class StoreDatabase



}   // end namespace SimpleMUD

#endif
