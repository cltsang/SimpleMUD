// MUD Programming
// Ron Penton
// (C)2003
// GameLoop.cpp - This class performs the game loop in SimpleMUD
// 
// 


#include "BasicLib/BasicLibTypes.h"
#include "GameLoop.h"
#include <fstream>
#include <string>

using namespace BasicLib;
using namespace SocketLib;

namespace SimpleMUD
{

sint64 DBSAVETIME = minutes( 15 );
sint64 ROUNDTIME  = seconds( 1 );
sint64 REGENTIME  = minutes( 2 );
sint64 HEALTIME   = minutes( 1 );


void GameLoop::Load()
{
    std::ifstream file( "game.data" );
    file >> std::ws;

    if( file.good() )
    {
        std::string temp;
        sint64 time;
        file >> temp;   extract( file, time );
        Game::GetTimer().Reset( time );

        file >> temp;   extract( file, m_savedatabases );
        file >> temp;   extract( file, m_nextround );
        file >> temp;   extract( file, m_nextregen );
        file >> temp;   extract( file, m_nextheal );
    }
    else
    {
        Game::GetTimer().Reset();
        m_savedatabases = DBSAVETIME;
        m_nextround = ROUNDTIME;
        m_nextregen = REGENTIME;
        m_nextheal = HEALTIME;
    }

    Game::Running() = true;
}


void GameLoop::Save()
{
    std::ofstream file( "game.data" );

    // save the game time
    file << "[GAMETIME]      "; insert( file, Game::GetTimer().GetMS() ); file << "\n";
    file << "[SAVEDATABASES] "; insert( file, m_savedatabases );          file << "\n";
    file << "[NEXTROUND]     "; insert( file, m_nextround );              file << "\n";
    file << "[NEXTREGEN]     "; insert( file, m_nextregen );              file << "\n";
    file << "[NEXTHEAL]      "; insert( file, m_nextheal );               file << "\n";
}

void GameLoop::Loop()
{
    if( Game::GetTimer().GetMS() >= m_nextround )
    {
        PerformRound();
        m_nextround += ROUNDTIME;
    }

    if( Game::GetTimer().GetMS() >= m_nextregen )
    {
        PerformRegen();
        m_nextregen += REGENTIME;
    }

    if( Game::GetTimer().GetMS() >= m_nextheal )
    {
        PerformHeal();
        m_nextheal += HEALTIME;
    }

    if( Game::GetTimer().GetMS() >= m_savedatabases )
    {
        SaveDatabases();
        m_savedatabases += DBSAVETIME;
    }
}


void GameLoop::LoadDatabases()
{
    Load();
    ItemDatabase::Load();
    PlayerDatabase::Load();
    RoomDatabase::LoadTemplates();
    RoomDatabase::LoadData();
	StoreDatabase::Load();
    EnemyTemplateDatabase::Load();
    EnemyDatabase::Load();
}


void GameLoop::SaveDatabases()
{
    Save();
    PlayerDatabase::Save();

}


void GameLoop::PerformRound()
{
    EnemyDatabase::iterator itr = EnemyDatabase::begin();
    sint64 now = Game::GetTimer().GetMS();

    while( itr != EnemyDatabase::end() )
    {
        if( now >= itr->NextAttackTime() && 
            itr->CurrentRoom()->Players().size() > 0 )
            Game::EnemyAttack( itr->ID() );
        ++itr;
    }
}

void GameLoop::PerformRegen()
{
    RoomDatabase::iterator itr = RoomDatabase::begin();
    
    while( itr != RoomDatabase::end() )
    {
        if( itr->SpawnWhich() != 0 &&
            itr->Enemies().size() < itr->MaxEnemies() )
        {
            EnemyDatabase::Create( itr->SpawnWhich(), itr->ID() );
            Game::SendRoom( red + bold + itr->SpawnWhich()->Name() + 
                            " enters the room!", itr->ID() );
        }

        ++itr;
    }
}


void GameLoop::PerformHeal()
{
    PlayerDatabase::iterator itr = PlayerDatabase::begin();
    while( itr != PlayerDatabase::end() )
    {
        if( itr->Active() )
        {
            itr->AddHitpoints( itr->GetAttr( HPREGEN ) );
            itr->PrintStatbar( true );
        }
        ++itr;
    }
}



}   // end namespace SimpleMUD

