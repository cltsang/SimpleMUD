// MUD Programming
// Ron Penton
// (C)2003
// Entity.h - This is a base class defining classes in SimpleMUD that may be
//             matched by name either partially or fully, and have ID's
// 
// 


#ifndef SIMPLEMUDENTITY_H
#define SIMPLEMUDENTITY_H

#include <string>
#include "BasicLib/BasicLib.h"


namespace SimpleMUD
{

typedef unsigned int entityid;
using std::string;

// --------------------------------------------------------------------
//  This class has a given name, and will allow you to match
//  the name with any other names either partially or fully.
//  It also has ID's
// --------------------------------------------------------------------
class Entity
{
public:


    // --------------------------------------------------------------------
    //  Constructor
    // --------------------------------------------------------------------
    inline Entity()
    {
        m_name = "UNDEFINED";
        m_id = 0;
    }

    // --------------------------------------------------------------------
    //  Retrieves a reference to the name, which is modifiable.
    // --------------------------------------------------------------------
    inline string& Name()   { return m_name; }


    // --------------------------------------------------------------------
    //  returns the "comparison name", which is lowercased. This is because
    //  all comparisons are case-insensitive.
    // --------------------------------------------------------------------
    inline string CompName() const
    { 
        return BasicLib::LowerCase( m_name );
    }

    // --------------------------------------------------------------------
    //  Compares the name with a full match. Names must be identical except
    //  for case.
    // --------------------------------------------------------------------
    inline bool MatchFull( const string& p_str ) const
    {
        return CompName() == BasicLib::LowerCase( p_str );
    }

    // --------------------------------------------------------------------
    //  Performs a partial match on the name, where it will match the first
    //  n characters from m_name with a n-character-long string p_name.
    //  Case-insensitive.
    //  IE: "J", "j", "JO" all match "John".  "o", "ohn" will not.
    // --------------------------------------------------------------------
    inline bool Match( const string& p_str ) const
    {
        // match on empty strings
        if( p_str.size() == 0 )
            return true;

        // grab the comparison name
        string name = CompName();
        string search = BasicLib::LowerCase( p_str );

        // find the position of a match
        size_t pos = name.find( search );

        // perform as many search passes as needed
        // for example, if the user types "st" when searching a "rusty stake",
        // only one pass would find the 'st' in "rusty", and determine that
        // there is no match. That's why more than one pass is needed.
        while( pos != string::npos )
        {
            // match found at beginning, or match found at beginning of a word, 
            // therefore, return true.
            if( pos == 0 || m_name[pos-1] == ' ' )
                return true;

            // perform another search, starting at where the last one left off
            pos = name.find( search, pos + 1 );
        }

        // no matches
        return false;
    }

    // --------------------------------------------------------------------
    //  Returns the ID for the entity
    // --------------------------------------------------------------------
    inline entityid& ID()    { return m_id; }


protected:
    string m_name;
    entityid m_id;

};  // end class Entity



struct matchentityfull
{ 
    matchentityfull( const string& p_str )
        : m_str( p_str ) { /* do nothing */ }

    bool operator() ( const Entity& p_entity )
    {
        return p_entity.MatchFull( m_str );
    }

    bool operator() ( Entity* p_entity )
    {
        return p_entity != 0 && p_entity->MatchFull( m_str );
    }


    string m_str;
};


struct matchentity
{ 
    matchentity( const string& p_str )
        : m_str( p_str ) { /* do nothing */ }

    bool operator() ( const Entity& p_entity )
    {
        return p_entity.Match( m_str );
    }

    bool operator() ( Entity* p_entity )
    {
        return p_entity != 0 && p_entity->Match( m_str );
    }

    string m_str;
};







}   // end namespace SimpleMUD


#endif
