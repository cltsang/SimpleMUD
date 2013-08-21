

#include <string>
#include <iostream>

#include "Room.h"
#include "Enemy.h"
#include "BasicLib/BasicLib.h"

using std::ostream;
using std::istream;
using BasicLib::extract;

namespace SimpleMUD
{

Room::Room()
{
    m_type = SAVEROOM;
	m_area = METROPOLIS;
    
    m_description = "UNDEFINED";

    for( int d = 0; d < NUMDIRECTIONS; d++ )
        m_rooms[d] = 0;

    m_spawnwhich = 0;
    m_maxenemies = 0;
    m_money = 0;
}

void Room::AddPlayer( player p_player )
{
    m_players.push_back( p_player );
}

void Room::RemovePlayer( player p_player )
{
    m_players.erase( std::find( m_players.begin(), 
                                m_players.end(), 
                                (entityid)p_player ) );
}


item Room::FindItem( const string& p_item )
{
    std::list<item>::iterator itr = 
            BasicLib::double_find_if(
                m_items.begin(),
                m_items.end(),
                matchentityfull( p_item ),
                matchentity( p_item ) );

    if( itr == m_items.end() )
        return 0;

    return *itr;
}

void Room::AddItem( item p_item )
{
    // remove the first (oldest) item if there's too many in the room.
    if( m_items.size() >= 32 )
        m_items.pop_front();

    // add the new item.
    m_items.push_back( p_item );
}

void Room::RemoveItem( item p_item )
{
    m_items.erase( std::find( m_items.begin(), 
                              m_items.end(), 
                              (entityid)p_item ) );
}


enemy Room::FindEnemy( const string& p_enemy )
{
    std::list<enemy>::iterator itr = 
            BasicLib::double_find_if(
                m_enemies.begin(),
                m_enemies.end(),
                matchentityfull( p_enemy ),
                matchentity( p_enemy ) );

    if( itr == m_enemies.end() )
        return 0;

    return *itr;
}


void Room::AddEnemy( enemy p_enemy )
{
    m_enemies.push_back( p_enemy );
}

void Room::RemoveEnemy( enemy p_enemy )
{
    m_enemies.erase( std::find( m_enemies.begin(), 
                                m_enemies.end(), 
                                (entityid)p_enemy ) );
}


void Room::LoadTemplate( istream& p_stream )
{
    string temp;

    p_stream >> temp >> std::ws;    std::getline( p_stream, m_name );
    p_stream >> temp >> std::ws;    std::getline( p_stream, m_description );
    p_stream >> temp >> temp;       m_type = GetRoomType( temp );
//    p_stream >> temp >> m_data;

    for( int d = 0; d < NUMDIRECTIONS; d++ )
        p_stream >> temp >> m_rooms[d];

    p_stream >> temp >> m_spawnwhich;
    p_stream >> temp >> m_maxenemies;
}

void Room::LoadData( istream& p_stream )
{
    string temp;
    p_stream >> temp;

    // clear all the existing items, then load in all the new items
    m_items.clear();        
    entityid last;
    while( extract( p_stream, last ) != 0 )
        m_items.push_back( last );

    // load in the money
    p_stream >> temp;   p_stream >> m_money;
}

void Room::SaveData( ostream& p_stream )
{
    p_stream << "[ITEMS] ";
    
    std::list<item>::iterator itr = m_items.begin();
    while( itr != m_items.end() )
    {
        p_stream << *itr << " ";
        ++itr;
    }

    p_stream << "0\n";

    p_stream << "[MONEY] " << m_money << "\n";
}



}   // end namespace SimpleMUD

