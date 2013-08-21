// MUD Programming
// Ron Penton
// (C)2003
// Train.cpp - This class is the training handler for SimpleMUD.
// 
// 

#include "BasicLib/BasicLib.h"
#include "Train.h"
#include "PlayerDatabase.h"

using namespace SocketLib;

namespace SimpleMUD
{

// ------------------------------------------------------------------------
//  This handles incomming commands. Anything passed into this function
//  is assumed to be a complete command from a client.
// ------------------------------------------------------------------------
void Train::Handle( string p_data )
{
    p_data = BasicLib::LowerCase( ParseWord( p_data, 0 ) );

    Player& p = *m_player;

	 // if quit, 1) save the player to database
	if(p_data == "quit"){
		PlayerDatabase::SavePlayer(p.ID());
    //          2) RemoveHandler() and return to Game
		p.Conn()->RemoveHandler();
	}
	
	 // if 1, 2, 3, 1) Minus player p's statPoints by 1
	if(p_data=="1" || p_data=="2" || p_data=="3"){
		p.StatPoints() -= 1;
	 //             2) Add 1 to p's base attribute
		if(p_data=="1")
				p.SetBaseAttr(STRENGTH,p.GetBaseAttr(STRENGTH)+1);
		else if(p_data=="2")
				p.SetBaseAttr(HEALTH,p.GetBaseAttr(HEALTH)+1);
		else if(p_data=="3")
				p.SetBaseAttr(AGILITY,p.GetBaseAttr(AGILITY)+1);
	 //             3) PrintStats again
		PrintStats(false);
		if(p.StatPoints() == 0){
			PlayerDatabase::SavePlayer(p.ID());
			p.Conn()->RemoveHandler();
		}
	}

}


// ------------------------------------------------------------------------
//  This notifies the handler that there is a new connection
// ------------------------------------------------------------------------
void Train::Enter()
{
    Player& p = *m_player;
	p.SendString(bold +
		white + "You must train your character with your desired stats,\r\n" +
		"before you enter the realm.\r\n");
	// set p newbie = false
	p.Newbie() = false;
	// call PrintStats
	PrintStats(false);
}



// ------------------------------------------------------------------------
//  This function prints out your statistics.
// ------------------------------------------------------------------------
void Train::PrintStats( bool p_clear )
{
    using BasicLib::tostring;

    Player& p = *m_player;

    if( p_clear )
    {
        p.SendString( clearscreen );
    }

	 // SendString
	p.SendString(bold + white +
		"--------------------------------- Your Stats ----------------------------------\r\n" +
		"Player:            " + p.Name() + "\r\n" +
		"Level:             " + tostring(p.Level()) + "\r\n" +
		"Stat Points Left:  " + tostring(p.StatPoints()) + "\r\n" +
		"1) Strength:       " + tostring(p.GetBaseAttr(STRENGTH)) + "\r\n" +
		"2) Health:         " + tostring(p.GetBaseAttr(HEALTH)) + "\r\n" +
		"3) Agility:        " + tostring(p.GetBaseAttr(AGILITY)) + "\r\n" +
        "-------------------------------------------------------------------------------\r\n" +
		"Enter 1, 2, or 3 to add a stat point, or \"quit\" to enter the realm: "
	);
}


}   // end namespace SimpleMUD

