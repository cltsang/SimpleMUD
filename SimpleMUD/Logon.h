// MUD Programming
// Ron Penton
// (C)2003
// Logon.h - This class is the logon handler for SimpleMUD.
// 
// 

#ifndef SIMPLEMUDLOGON_H
#define SIMPLEMUDLOGON_H

#include "SocketLib/SocketLib.h"
#include "SimpleMUDLogs.h"
#include "Game.h"
#include <string>

using SocketLib::Telnet;
using SocketLib::Connection;
using std::string;

namespace SimpleMUD
{


enum LogonState 
{
    NEWCONNECTION,           // first state
    NEWUSER,                 // new user, enter desired name
    ENTERNEWPASS,            // new user, enter desired password
    ENTERPASS                // existing user, enter password
};



class Logon : public Telnet::handler
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
    Logon( Connection<Telnet>& p_conn )
        : thandler( p_conn )
    {
        m_state = NEWCONNECTION;
        m_errors = 0;
    }


    // ------------------------------------------------------------------------
    //  Handler Functions
    // ------------------------------------------------------------------------
    void Handle( string p_data );
    void Enter();
    void Leave() {};
    void Hungup() 
    {
        USERLOG.Log(  
            SocketLib::GetIPString( m_connection->GetRemoteAddress() ) + 
            " - hung up in login state." );
    };
 
    void Flooded() 
    {
        USERLOG.Log(  
            SocketLib::GetIPString( m_connection->GetRemoteAddress() ) + 
            " - flooded in login state." );
    };

    // ------------------------------------------------------------------------
    //  This changes the game state so that the player enters the game.
    // ------------------------------------------------------------------------
    void GotoGame( bool p_newbie = false );

    // ------------------------------------------------------------------------
    //  This tells a connection that there is no room for the connection and
    //  that it will be terminated.
    // ------------------------------------------------------------------------
    static void NoRoom( Connection<Telnet>& p_connection )
    {
        static string msg = "Sorry, there is no more room on this server.\r\n";
        try
        {
            p_connection.Send( msg.c_str(), (int)msg.size() );
        }
        catch( SocketLib::Exception )
        {
            // do nothing here; probably an exploiter if sending that data
            // causes an exception.
        }
    }

    // ------------------------------------------------------------------------
    //  This checks if a user name is acceptible.
    // ------------------------------------------------------------------------
    static bool AcceptibleName( const string& p_name );

protected:

    LogonState m_state;
    int m_errors;               // how many times has an invalid answer been
                                // entered?

    string m_name;              // name
    string m_pass;              // password


};  // end class Logon



}   // end namespace SimpleMUD

#endif
