// MUD Programming
// Ron Penton
// (C)2003
// Store.h - The class defining Stores in the SimpleMUD
// 
// 


#ifndef SIMPLEMUDSTORE_H
#define SIMPLEMUDSTORE_H

#include <string>
#include <iostream>
#include <list>
#include <utility>
#include "BasicLib/BasicLib.h"

#include "Entity.h"
#include "Item.h"
#include "DatabasePointer.h"

using std::ostream;
using std::istream;
using namespace BasicLib;

namespace SimpleMUD
{

// --------------------------------------------------------------------
//  Class that stores information about stores
// --------------------------------------------------------------------
class Store : public Entity
{
public:

	typedef std::pair< item, int > product;
	typedef std::vector<product>::iterator iterator;

    iterator find( const string& p_item )
    {
        iterator itr;

		for(itr=begin(); itr!=end(); ++itr){
			if(itr->first->Name() == p_item)
				return itr;
		}

		return end();
    }

    iterator begin()    { return m_product.begin(); }
    iterator end()      { return m_product.end(); }
    size_t size()       { return m_product.size(); }

    bool has( entityid p_item )
    {
        iterator itr;

		for(itr=begin(); itr!=end(); ++itr){
			if(itr->first->ID() == p_item)
				return true;
		}

		return false;
    }

	std::vector<product>& ProductList() { return m_product; }

    friend istream& operator>>( istream& p_stream, Store& s );


protected:
    std::vector<product> m_product;

};  // end class Room


/*
inline istream& operator>>( istream& p_stream, Store& s )
{
    string temp;

    // read name
    p_stream >> temp >> std::ws;    std::getline( p_stream, s.Name() );

    // clear the item listing on load, in case this is being reloaded
    s.m_items.clear();      

    // now load in the item listing.
    entityid last;
    p_stream >> temp;                           // chew up "[ITEMS]" tag

    while( extract( p_stream, last ) != 0 )     // loop while item ID's valid
        s.m_items.push_back( last );            // add item
    
    return p_stream;
}
*/



}   // end namespace SimpleMUD


#endif
