// MUD Programming
// Ron Penton
// (C)2003
// ItemDatabase.cpp - The class that stores all of the items within the MUD
// 
// 

#include <fstream>
#include "BasicLib/BasicLib.h"
#include "SimpleMUDLogs.h"
#include "ItemDatabase.h"
#include "mysql++.h"

using BasicLib::LowerCase;
using BasicLib::tostring;

namespace SimpleMUD
{

// declare the static map of the Item database.
std::map<entityid, Item> EntityDatabase<Item>::m_map;


bool ItemDatabase::Load() {
	// Connect to the onlinegame database
	// In case of any error,
	//    USERLOG.Log ( std::string );
	//    return false;
	mysqlpp::Connection con;
	if(!con.connect("onlinegame", "localhost", "root")){
		USERLOG.Log(std::string(con.error()));
		return false;
	}

	mysqlpp::Query query = con.query();
	// Select * from item
	query << "SELECT * FROM item";
	mysqlpp::StoreQueryResult res = query.store();
	// For every row,
	for(mysqlpp::StoreQueryResult::iterator itr=res.begin(); itr!=res.end(); ++itr){
		mysqlpp::Row row = *itr;
		entityid id = row["id"];
	    m_map[id].ID() = id;
	    m_map[id].Name() = row["name"];
		m_map[id].Type() = GetItemType((std::string) row["type"]);
		m_map[id].Min() = row["min"];
		m_map[id].Max() = row["max"];
		m_map[id].Speed() = row["speed"];
		m_map[id].Price() = row["price"];
	//     For every attribute i,
		for(int i=0; i<NUMATTRIBUTES; i++){
	//         get the attribute name by ATTRIBUTESTRINGS[i].c_str()
			m_map[i].m_attributes[i] = row[ATTRIBUTESTRINGS[i].c_str()];
			// m_map[i].m_attributes[0] = row["STRENGTH"];
		}
	}
	return true;
}




}   // end namespace SimpleMUD
