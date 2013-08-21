// MUD Programming
// Ron Penton
// (C)2003
// PlayerDatabase.h - The class that stores all of the players within the MUD
// 
// 

#ifndef SIMPLEMUDPLAYERDATABASE_H
#define SIMPLEMUDPLAYERDATABASE_H


#include <cmath>
#include <string>
#include <map>
#include <set>

#include "EntityDatabase.h"
#include "Player.h"

namespace SimpleMUD
{

class PlayerDatabase : public EntityDatabase<Player>
{
public:

    static bool Load();
    static bool Save();
    static bool AddPlayer( Player& p_player );


    // helpers
    static inline string PlayerFileName( const string& p_name );
    static void LoadPlayer( string p_name );
    static void SavePlayer( entityid p_player );


    static entityid LastID()  
    { 
        // since the ID's are sorted in ascending order within the map,
        // the highest ID will be the ID of the last item.
        return m_map.rbegin()->first;
    }

    static iterator findactive( const std::string& p_name )
    {
        return BasicLib::double_find_if( 
            begin(), end(), matchentityfull( p_name ),
            matchentity( p_name ), playeractive() );
    }

    static iterator findloggedin( const std::string& p_name )
    {
        return BasicLib::double_find_if( 
            begin(), end(), matchentityfull( p_name ),
            matchentity( p_name ), playerloggedin() );
    }

    static void Logout( entityid p_player );

};  // end class PlayerDatabase



inline string PlayerDatabase::PlayerFileName( const string& p_name )
{
    return string( "players/" + p_name + ".plr" );
}



}   // end namespace SimpleMUD

#endif
