// MUD Programming
// Ron Penton
// (C)2003
// StoreDatabase.cpp - The class that stores all of the stores within the MUD
// 
// 

#include <fstream>
#include "BasicLib/BasicLib.h"
#include "SimpleMUDLogs.h"
#include "StoreDatabase.h"
#include "mysql++.h"

using BasicLib::LowerCase;
using BasicLib::tostring;
using std::string;
using std::ifstream;
using std::ofstream;

namespace SimpleMUD
{

// declare the static map of the Item database.
std::map<entityid, Store> EntityDatabase<Store>::m_map;


bool StoreDatabase::Load()
{
	mysqlpp::Connection con;
	if(!con.connect("onlinegame", "localhost", "root")){
		USERLOG.Log((std::string) con.error());
		return false;
	}
	mysqlpp::Query query = con.query();
	query << "SELECT * FROM store_items";
	mysqlpp::StoreQueryResult res = query.store();

	for(mysqlpp::StoreQueryResult::iterator itr=res.begin(); itr!=res.end(); ++itr){
		mysqlpp::Row row = *itr;
		entityid id = row["roomid"];
		m_map[id].ID() = id;
		m_map[id].ProductList().push_back(std::make_pair(row["itemid"], row["pricefloat"]));
	}
	return true;
}






}   // end namespace SimpleMUD
