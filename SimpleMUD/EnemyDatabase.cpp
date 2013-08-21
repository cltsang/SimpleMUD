// MUD Programming
// Ron Penton
// (C)2003
// EnemyDatabase.h - The class that stores all of the enemies within the MUD
// 
// 

#include <fstream>
#include <vector>
#include "BasicLib/BasicLib.h"
#include "SimpleMUDLogs.h"
#include "EnemyDatabase.h"
#include "Room.h"
#include "mysql++.h"

using BasicLib::LowerCase;
using BasicLib::tostring;
using std::string;
using std::ifstream;
using std::ofstream;

namespace SimpleMUD
{

// declare the static vector of the enemy template database.
std::vector<EnemyTemplate> EntityDatabaseVector<EnemyTemplate>::m_vector;

// declare the static map of the enemy instance database.
std::map<entityid, Enemy> EntityDatabase<Enemy>::m_map;


void EnemyTemplateDatabase::Load()
{
	mysqlpp::Connection con;
	if(!con.connect("onlinegame", "localhost", "root")){
		USERLOG.Log((std::string) con.error());
		return;
	}
	mysqlpp::Query query = con.query();
	query << "SELECT * FROM enemy";
	mysqlpp::StoreQueryResult res = query.store();
	// The enemy template is a vector m_vector
	// Resize the m_vector to appropiate value
	m_vector.resize(res.size()+1);
	for(mysqlpp::StoreQueryResult::iterator itr=res.begin(); itr!=res.end(); ++itr){
		mysqlpp::Row row = *itr;
		entityid id = row["id"];
	// Make a new enemy template by m_vector[id].ID() = id;
		m_vector[id].ID() = id;
	// Read every enemy template into m_vector[id]
		m_vector[id].Name() = row["name"];
		m_vector[id].m_hitpoints = row["hitpoints"];
		m_vector[id].m_accuracy = row["accuracy"];
		m_vector[id].m_dodging = row["dodging"];
		m_vector[id].m_strikedamage = row["strikedamage"];
		m_vector[id].m_damageabsorb = row["damageabsorb"];
		m_vector[id].m_experience = row["experience"];
		m_vector[id].m_weapon = row["weapon"];
		m_vector[id].m_moneymin = row["moneymin"];
		m_vector[id].m_moneymax = row["moneymax"];
	// Check istream& operator>>( istream& p_stream, EnemyTemplate& t ) in Enemy.cpp
	}
	
	query << "SELECT * FROM enemy_loot";
	res = query.store();
	for(mysqlpp::StoreQueryResult::iterator itr=res.begin(); itr!=res.end(); ++itr){
		mysqlpp::Row row = *itr;
		m_vector[row["id"]].m_loot.push_back(std::make_pair(row["itemid"], row["percentage"]));
	}
}

void EnemyDatabase::Create( entityid p_template, room p_room )
{
    entityid id = FindOpenID();
    Enemy& e = m_map[id];

    e.ID() = id;
    e.LoadTemplate( p_template );
    e.CurrentRoom() = p_room;

    p_room->AddEnemy( id );
}


void EnemyDatabase::Delete( enemy p_enemy )
{
    p_enemy->CurrentRoom()->RemoveEnemy( p_enemy );
    m_map.erase( p_enemy );
}


void EnemyDatabase::Load()
{
	mysqlpp::Connection con;
	if(!con.connect("onlinegame", "localhost", "root")){
		USERLOG.Log((std::string) con.error());
		return;
	}
	mysqlpp::Query query = con.query();
	query << "SELECT * FROM enemy_instance";
	mysqlpp::StoreQueryResult res = query.store();
	// The enemy instance is a map m_map
	for(mysqlpp::StoreQueryResult::iterator itr=res.begin(); itr!=res.end(); ++itr){
		mysqlpp::Row row = *itr;
		entityid id = row["id"];
	// Make a new enemy instance by m_map[id].ID() = id;
		m_map[id].ID() = id;
	// Read every enemy instance into m_map[id]
		m_map[id].LoadTemplate(enemytemplate(row["templateid"]));
		m_map[id].HitPoints() = row["hitpoints"];
		m_map[id].CurrentRoom() = row["room"];
		m_map[id].NextAttackTime() = row["nextattacktime"];
	// Add the enemy into the room by m_map[id].CurrentRoom()->AddEnemy( id );
		m_map[id].CurrentRoom()->AddEnemy(id);
	}
}



}   // end namespace SimpleMUD
