
#include <fstream>
#include <vector>
#include "BasicLib/BasicLib.h"
#include "SimpleMUDLogs.h"
#include "RoomDatabase.h"
#include "mysql++.h"

using BasicLib::LowerCase;
using BasicLib::tostring;
using std::string;
using std::ifstream;
using std::ofstream;


namespace SimpleMUD
{

// declare the static vector of the room database.
std::vector<Room> EntityDatabaseVector<Room>::m_vector;


void RoomDatabase::LoadTemplates()
{
	// Connect to the database
	mysqlpp::Connection con;
	if(!con.connect("onlinegame", "localhost", "root")){
		USERLOG.Log((std::string) con.error());
		return;
	}
	mysqlpp::Query query = con.query();
	// The room vector is m_vector
	// Resize the m_vector to appropiate value
	query << "SELECT * FROM room";
	mysqlpp::StoreQueryResult res = query.store();
	m_vector.resize(res.size()+1);
	// Make a new room object by m_vector[id].ID() = id;
	// Read every room template into m_vector[id]
	for(mysqlpp::StoreQueryResult::iterator itr=res.begin(); itr!=res.end(); ++itr){
		mysqlpp::Row row = *itr;
		entityid id = row["id"];
		m_vector[id].ID() = id;
	// Check Room::LoadTemplate() for example
   // Fill the m_name, m_description, m_type, m_data, m_rooms[], m_spawnwhich, m_maxenemies 
		m_vector[id].Name() = row["name"];
		m_vector[id].Description() = row["description"];
		m_vector[id].Type() = GetRoomType((std::string)row["type"]);
		m_vector[id].InArea() = GetArea((std::string)row["area"]);
		m_vector[id].m_rooms[NORTH] = row["north"];
		m_vector[id].m_rooms[EAST] = row["east"];
		m_vector[id].m_rooms[SOUTH] = row["south"];
		m_vector[id].m_rooms[WEST] = row["west"];
		m_vector[id].SpawnWhich() = row["enemy"];
		m_vector[id].MaxEnemies() = row["maxenemies"];
	}
}

void RoomDatabase::LoadData()
{
	// Connect to the database
	mysqlpp::Connection con;
	if(!con.connect("onlinegame", "localhost", "root")){
		USERLOG.Log((std::string) con.error());
		return;
	}
	mysqlpp::Query query = con.query();
	query << "SELECT * FROM room_volatile";
	mysqlpp::StoreQueryResult res = query.store();
	// Read in room volatile data to every m_vector[id] object
	for(mysqlpp::StoreQueryResult::iterator itr=res.begin(); itr!=res.end(); ++itr){
		mysqlpp::Row row = *itr;
		entityid id = row["id"];
	// Check Room::LoadData() for example
	// Clear all existing items
		m_vector[id].m_items.clear();
	// Load m_items and m_money
		entityid current, last=0;
		std::stringstream items;
		items << row["items"];
		while(BasicLib::extract(items, current) != last){
			last = current;
			m_vector[id].m_items.push_back( last );
		}
		m_vector[id].Money() = row["money"];
	}
}

}   // end namespace SimpleMUD
