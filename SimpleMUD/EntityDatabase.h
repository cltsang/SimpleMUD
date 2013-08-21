// MUD Programming
// Ron Penton
// (C)2003
// EntityDatabase.h - A generic class that will store Entity objects by their ID
//                    and will allow searching for those items.
// 
// 

#ifndef SIMPLEMUDENTITYDATABASE_H
#define SIMPLEMUDENTITYDATABASE_H


#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <iterator>
#include "Entity.h"

using std::istream;
using std::ostream; 


namespace SimpleMUD
{


template< class datatype >
class EntityDatabase
{
public:

    typedef std::map<entityid, datatype> container;
	typedef typename std::map<entityid, datatype>::iterator cIter;

    // --------------------------------------------------------------------
    //  The inner iterator class, used to iterate through the database.
    // --------------------------------------------------------------------
    class iterator : public cIter
    {
    public:

        // --------------------------------------------------------------------
        // NOTE: the constructors are needed as a result of VC6 sucking.
        // Have I mentioned that VC6 sucks yet?
        // --------------------------------------------------------------------
        iterator() {};  // default constructor
        iterator( const cIter& p_itr ) // copy constructor
        {
            cIter& itr = *this; // also needed because VC6 sucks
            itr = p_itr;
        }

        // --------------------------------------------------------------------
        //  "dereferences" the iterator to return a reference to the
        //  object that it points to.
        // --------------------------------------------------------------------
        inline datatype& operator*()
        {
            cIter& itr = *this; // also needed because VC6 sucks
            return itr->second;
        }

        // --------------------------------------------------------------------
        //  the "pointer-to-member" operator, which will allow you to use it on
        //  iterators like this: itr->Function();
        // --------------------------------------------------------------------
        inline datatype* operator->()
        {
            cIter& itr = *this; // also needed because VC6 sucks
            return &(itr->second);
        }
    };  // end iterator inner class


    // --------------------------------------------------------------------
    //  returns an iterator pointing to the first item
    // --------------------------------------------------------------------
    inline static iterator begin()
    {
        return iterator( m_map.begin() );
    }

    // --------------------------------------------------------------------
    //  returns the "invalid" iterator, the iterator that points one 
    //  past the end of the DB. Used for comparisons.
    // --------------------------------------------------------------------
    inline static iterator end()
    {
        return iterator( m_map.end() );
    }

    // --------------------------------------------------------------------
    //  finds entity based in ID
    // --------------------------------------------------------------------
    inline static iterator find( entityid p_id )
    {
        return iterator( m_map.find( p_id ) );
    }

    // --------------------------------------------------------------------
    //  finds entity matching name exactly
    // --------------------------------------------------------------------
    static iterator findfull( const std::string& p_name )
    {
        return std::find_if( begin(), end(), matchentityfull( p_name ) );
    }

    // --------------------------------------------------------------------
    //  finds entity matching name partially
    // --------------------------------------------------------------------
    static iterator find( const std::string& p_name )
    {
        return BasicLib::double_find_if( begin(), end(), 
            matchentityfull( p_name ),
            matchentity( p_name ) );
    }

    // --------------------------------------------------------------------
    //  gets an item based on ID.
    // --------------------------------------------------------------------
    inline static datatype& get( entityid p_id )
    {
        iterator itr = find( p_id );
        if( itr == end() )
            throw std::exception();

        return *itr;
    }


    inline static bool has( entityid p_id )
    {
        return ( m_map.find( p_id ) != m_map.end() );
    }

    inline static bool hasfull( std::string p_name )
    {
        return findfull( p_name ) != end();
    }

    inline static bool has( std::string p_name )
    {
        return find( p_name ) != end();
    }

    inline static int size() { return (int)m_map.size(); }


    static entityid FindOpenID()
    {
        if( m_map.size() == 0 )
            return 1;

        if( m_map.size() == m_map.rbegin()->first )
            return m_map.size() + 1;

        entityid openid = 0;
        entityid previous = 0;
        std::map<entityid,datatype>::iterator itr = m_map.begin();

        while( !openid )
        {
            if( itr->first != previous + 1 )
                openid = previous + 1;
            else
                previous = itr->first;
            ++itr;
        }

        return openid;
    }

protected:
    static std::map<entityid, datatype> m_map;

};  // end class EntityDatabase



template< typename datatype >
class EntityDatabaseVector
{
public:

    // internal iterator
    typedef typename std::vector<datatype>::iterator iterator;

    inline static iterator begin()  { return m_vector.begin() + 1; }
    inline static iterator end()    { return m_vector.end(); }
    inline static size_t size()     { return m_vector.size() - 1; }

    // --------------------------------------------------------------------
    //  gets an enemy based on ID.
    // --------------------------------------------------------------------
    inline static datatype& get( entityid p_id )
    {
        if( p_id >= m_vector.size() || p_id == 0 )
            throw std::exception();
        return m_vector[p_id];
    }


protected:
    static std::vector<datatype> m_vector;

};  // end class EntityDatabaseVector




}   // end namespace SimpleMUD

#endif
