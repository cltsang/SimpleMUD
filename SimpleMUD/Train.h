// MUD Programming
// Ron Penton
// (C)2003
// Train.h - This class is the training handler for SimpleMUD.
// 
// 

#ifndef SIMPLEMUDTRAIN_H
#define SIMPLEMUDTRAIN_H

#include <string>
#include "SocketLib/SocketLib.h"
#include "PlayerDatabase.h"

using SocketLib::Telnet;
using SocketLib::Connection;
using std::string;

namespace SimpleMUD
{


class Train : public Telnet::handler
{
    // typedef the class because MSVC6 isn't smart enough to construct
    // a "Telnet::handler" object, yet it will construct a typedef just
    // fine.  *boggle*
    typedef Telnet::handler thandler;
public:

    // ------------------------------------------------------------------------
    //  Construct the handler with a reference to the connection so that it can
    //  be used later on. Handlers are initialized only once, and cannot change
    //  connections. A flaw in MSVC6 neccessitated the typedeffing of
    //  Telnet::handler to thandler in order to call the base constructor.
    // ------------------------------------------------------------------------
    Train( Connection<Telnet>& p_conn, player p_player )
        : thandler( p_conn )
    {
        m_player = p_player;
    }


    // ------------------------------------------------------------------------
    //  standard handler commands
    // ------------------------------------------------------------------------
    void Handle( string p_data );
    void Enter();
    void Leave() {}
    void Hungup() { PlayerDatabase::Logout( m_player ); }
    void Flooded() { PlayerDatabase::Logout( m_player ); }

    // ------------------------------------------------------------------------
    //  This function prints out your statistics.
    // ------------------------------------------------------------------------
    void PrintStats( bool p_clear = true );

protected:

    player m_player;


};  // end class Train



}   // end namespace SimpleMUD

#endif
