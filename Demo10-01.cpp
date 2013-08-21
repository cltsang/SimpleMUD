// MUD Programming
// Ron Penton
// (C)2003
// Demo10-01.cpp - SimpleMUD - Enemies, Enemy Databases, and the Game Loop
// 
// 

#include <sstream>

#include "SocketLib/SocketLib.h"

#include "SimpleMUD/ItemDatabase.h"
#include "SimpleMUD/PlayerDatabase.h"
#include "SimpleMUD/RoomDatabase.h"
#include "SimpleMUD/EnemyDatabase.h"

#include "SimpleMUD/Logon.h"
#include "SimpleMUD/Game.h"
#include "SimpleMUD/GameLoop.h"

#include "SimpleMUD/SimpleMUDLogs.h"


using namespace SocketLib;
using namespace SimpleMUD;


int main()
{
    try
    {
        GameLoop gameloop;

        ListeningManager<Telnet, Logon> lm;
        ConnectionManager<Telnet, Logon> cm( 128, 60, 65536 );

        lm.SetConnectionManager( &cm );
        lm.AddPort( 5100 );


        while( Game::Running() )
        {
            lm.Listen();
            cm.Manage();
            gameloop.Loop();
            ThreadLib::YieldThread();
        }
    
    }

    catch( SocketLib::Exception& e )
    {
        ERRORLOG.Log( "Fatal Socket Error: " + e.PrintError() );
    }

    catch( ThreadLib::Exception& )
    {
        ERRORLOG.Log( "Fatal Thread Error" );
    }

    catch( std::exception& e )
    {
        ERRORLOG.Log( "Standard Error: " + std::string( e.what() ) );
    }

    catch( ... )
    {
        ERRORLOG.Log( "Unspecified Error" );
    }



}

