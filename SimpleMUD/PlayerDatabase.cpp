// MUD Programming
// Ron Penton
// (C)2003
// PlayerDatabase.cpp - The class that stores all of the players within the MUD
// 
// 

#include <fstream>
#include "BasicLib/BasicLib.h"
#include "SimpleMUDLogs.h"
#include "PlayerDatabase.h"
#include "mysql++.h"

using BasicLib::LowerCase;
using BasicLib::tostring;
using std::string;
using std::ifstream;
using std::ofstream;


namespace SimpleMUD
{

// declare the static variables of the player database.
std::map<entityid, Player> EntityDatabase<Player>::m_map;


void PlayerDatabase::LoadPlayer (string p_name) {
	// In case of any error in this class, 
	//    USERLOG.Log ( std::string );
	//    return false;

	// -> Create a connection to the database onlinegame
	mysqlpp::Connection con;
	if(!con.connect("onlinegame", "localhost", "root")){
		USERLOG.Log((std::string) con.error());
		return;
	}
	mysqlpp::Query query = con.query();
	// SELECT * FROM player where name is p_name
	query << "SELECT * FROM player WHERE name='" << p_name <<"'";
	mysqlpp::StoreQueryResult res = query.store();
	mysqlpp::StoreQueryResult::iterator itr=res.begin();
	mysqlpp::Row row = *itr;
	entityid id = row["id"];
	// Fill every field to m_map[id]
	m_map[id].ID() = id;
	m_map[id].Name() = row["name"];
	m_map[id].Password() = row["pass"];
	m_map[id].Rank() = GetRank((std::string)row["rank"]);
	m_map[id].StatPoints() = row["statpoints"];
	m_map[id].Experience() = row["experience"];
	m_map[id].Level() = row["level"];
	m_map[id].CurrentRoom() = row["room"];
	m_map[id].Money() = row["money"];
	m_map[id].HitPoints() = row["hitpoints"];
	m_map[id].NextAttackTime() = row["nextattacktime"];
	// Fill every attribute to m_map[id].m_baseattributes
	for(int i=0; i<NUMATTRIBUTES; i++){
		m_map[id].SetBaseAttr(i, row[ATTRIBUTESTRINGS[i].c_str()]);
	}
	m_map[id].Weapon() = row["weapon"];
	m_map[id].Armor() = row["armor"];

	// SELECT * FROM p_name_inventory
	query << "SELECT * FROM " << p_name << "_inventory";
	res = query.store();
	// File every inventory to m_map[id].m_inventory
	int i;
	for(i=0, itr=res.begin(); itr!=res.end(); ++itr, ++i){
		row = *itr;
		m_map[id].m_inventory[i] = row["inventory"];
	}

	m_map[id].RecalculateStats();
}

void PlayerDatabase::SavePlayer( entityid p_player ) {
    std::map<entityid, Player>::iterator itr = m_map.find( p_player );
    if( itr == m_map.end() )
        return;	
	Player *p = &itr->second;

	// -> Create a connection to the database onlinegame
	mysqlpp::Connection con;
	if(!con.connect("onlinegame", "localhost", "root")){
		USERLOG.Log((std::string) con.error());
		return;
	}
	mysqlpp::Query query = con.query();
	// replace player values ( .. 
	query << "REPLACE INTO player SET " <<
		"id = " << p->ID() << ", " <<
		"name='" << p->Name() << "', " <<
		"pass='" << p->Password() << "', " <<
		"rank='" << GetRankString(p->Rank()) << "', " <<
		"statpoints=" << p->StatPoints() << ", " <<
		"experience=" << p->Experience() << ", " <<
		"level=" << p->Level() << ", " <<
		"room=" << p->CurrentRoom() << ", " <<
		"money=" << p->Money() << ", " <<
		"hitpoints=" << p->HitPoints() << ", " <<
		"nextattacktime=" << p->NextAttackTime() << ", " <<
		"strength=" << p->GetBaseAttr(STRENGTH) << ", " <<
		"health=" <<  p->GetBaseAttr(HEALTH) << ", " <<
		"agility=" <<  p->GetBaseAttr(AGILITY) << ", " <<
		"maxhitpoints=" <<  p->GetBaseAttr(MAXHITPOINTS) << ", " <<
		"accuracy=" <<  p->GetBaseAttr(ACCURACY) << ", " <<
		"dodging=" <<  p->GetBaseAttr(DODGING) << ", " <<
		"strikedamage=" <<  p->GetBaseAttr(STRIKEDAMAGE) << ", " <<
		"damageabsorb=" <<  p->GetBaseAttr(DAMAGEABSORB) << ", " <<
		"hpregen=" << p->GetBaseAttr(HPREGEN) << ", " <<
		"weapon=" << p->Weapon() << ", " <<
		"armor=" << p->Armor();
	query.exec();

	// Check if table playername_inventory is here
	query << "SHOW TABLES";
	bool tableExists = false;
	string tableName = p->Name() + "_inventory";
	mysqlpp::StoreQueryResult res = query.store();
	for(mysqlpp::StoreQueryResult::iterator itr=res.begin(); itr!=res.end(); ++itr){
		mysqlpp::Row row = *itr;
		if(row["Tables_in_onlinegame"] == tableName){
			tableExists = true;
			break;
		}
	}
	// create the inventory table if not exist
	if(!tableExists){
		query << "CREATE TABLE " << tableName << "(" <<
			"id int auto_increment," <<
			"inventory int," <<
			"primary key(id)," <<
			"foreign key(id) references item(id)" <<
			")";
		query.exec();
	}
	// replace every row in playername_inventory
//	query << "TRUNCATE TABLE " << tableName;
//	query.exec();

	query << "REPLACE INTO " << tableName << " VALUES ";
	int i=1;
	for(; i<PLAYERITEMS; ++i){
		query << "(" << i << ", " << p->m_inventory[i] << "),";
	}
	query << "(" << i << ", " << p->m_inventory[i] << ")";
	query.exec();
}


bool PlayerDatabase::Load() {
	// -> Create a connection to the database onlinegame
	mysqlpp::Connection con;
	if(!con.connect("onlinegame", "localhost", "root")){
		USERLOG.Log((std::string) con.error());
		return false;
	}
	mysqlpp::Query query = con.query();
	// SELECT * FROM player_name
	query << "SELECT * FROM player_name";
	mysqlpp::StoreQueryResult res = query.store();
    // For each name,
	for(mysqlpp::StoreQueryResult::iterator itr=res.begin(); itr!=res.end(); ++itr){
		mysqlpp::Row row = *itr;
	//     LoadPlayer( name );
		LoadPlayer(row["name"].c_str());
	}
	return true;
}


bool PlayerDatabase::Save() {
	// -> Create a connection to the database onlinegame
	mysqlpp::Connection con;
	if(!con.connect("onlinegame", "localhost", "root")){
		USERLOG.Log((std::string) con.error());
		return false;
	}
	// iterator itr = begin();
	std::map<entityid, Player>::iterator itr = m_map.begin();
	// while (itr != end() ) {
	while(itr != m_map.end()){
		// replace player_name values ( .. 
		mysqlpp::Query query = con.query();
		query << "REPLACE INTO player_name SET " <<
			"id = " << itr->first << ", "
			"name = " << itr->second.Name();
		// SavePlayer ( itr->ID() );
		SavePlayer(itr->first);
		// ++itr;
		++itr;
	//}
	}
	return true;
}
    
bool PlayerDatabase::AddPlayer( Player & p_player ) {
    if( has( p_player.ID() ) )
        return false;
    if( hasfull( p_player.Name() ) )
        return false;

    // insert the player into the map
    m_map[p_player.ID()] = p_player;

	// -> Create a connection to the database
	mysqlpp::Connection con;
	if(!con.connect("onlinegame", "localhost", "root")){
		USERLOG.Log((std::string) con.error());
		return false;
	}
	mysqlpp::Query query = con.query();
	// insert into player_name values (.. 
	query << "INSERT INTO player_name VALUES (null, '" + p_player.Name() + "')";
	query.exec();
    // SavePlayer( p_player.ID() );
	SavePlayer(p_player.ID());
	return true;
}


void PlayerDatabase::Logout( entityid p_player )
{
    Player& p = get( p_player );

    USERLOG.Log(  
        SocketLib::GetIPString( p.Conn()->GetRemoteAddress() ) + 
        " - User " + p.Name() + " logged off." );

    p.Conn() = 0;
    p.LoggedIn() = false;
    p.Active() = false;

    // make sure the player is saved to disk
    SavePlayer( p_player );
}


}   // end namespace SimpleMUD
