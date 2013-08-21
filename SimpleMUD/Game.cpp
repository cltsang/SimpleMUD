// MUD Programming
// Ron Penton
// (C)2003
// Game.cpp - This class is the game handler for SimpleMUD.
// 
// 

#include "Missions.h"
#include "Logon.h"
#include "Game.h"
#include "Train.h"
#include "ItemDatabase.h"
#include "PlayerDatabase.h"
#include "RoomDatabase.h"
#include "StoreDatabase.h"
#include "EnemyDatabase.h"
#include "BasicLib/BasicLib.h"
#include "../Utils.h"
#include "Skills.h"

using namespace SocketLib;
using namespace BasicLib;
using std::string;

namespace SimpleMUD
{	
	// declare the static instance of the timer
	BasicLib::Timer Game::s_timer;
	bool Game::s_running = false;
	
	// ------------------------------------------------------------------------
	//  This handles incomming commands. Anything passed into this function
	//  is assumed to be a complete command from a client.
	// ------------------------------------------------------------------------
	void Game::Handle( string p_data )
	{
		Player& p = *m_player;
		
		// check if the player wants to repeat a command
		if( p_data == "/" )
		{
			p_data = m_lastcommand;
		}
		else
		{
			// if not, record the command.
			m_lastcommand = p_data;
		}


		
		// get the first word and lowercase it.
		string firstword = BasicLib::LowerCase( ParseWord( p_data, 0 ) );
		
		
		// ------------------------------------------------------------------------
		//  REGULAR access commands
		// ------------------------------------------------------------------------
		if( firstword == "chat" || firstword == ":" )
		{
			string text = RemoveWord( p_data, 0 );
			SendGame( magenta + bold + p.Name() + " chats: " + white + text );
			return;
		}
		
		if( firstword == "experience" || firstword == "exp" )
		{
			p.SendString( PrintExperience() );
			return;
		}
		
		if( firstword == "help" || firstword == "commands" )
		{
			p.SendString( PrintHelp( p.Rank() ) );
			return;
		}
		
		if( firstword == "inventory" || firstword == "inv" )
		{
			p.SendString( PrintInventory() );
			return;
		}
		
		if( firstword == "quit" )
		{
			m_connection->Close();
			LogoutMessage( p.Name() + " has left the realm." );
			return;
		}
		
		if( firstword == "remove" )
		{
			RemoveItem( ParseWord( p_data, 1 ) );
			return;
		}
		
		if( firstword == "stats" || firstword == "st" )
		{
			p.SendString( PrintStats() );
			return;
		}

		if( firstword == "train" )
		{
			if(p.StatPoints() > 0)
				GotoTrain();
			else
				p.SendString( bold + red + "You don't have StatPoints left.");
			return;
		}
		
		if( firstword == "time" )
		{
			p.SendString( bold + cyan + 
						 "The current system time is: " + BasicLib::TimeStamp() + 
						 " on " + BasicLib::DateStamp() + 
						 "\r\nThe system has been up for: " 
						 + s_timer.GetString() + "." );
			return;
		}
		
		if( firstword == "use" )
		{
			UseItem( RemoveWord( p_data, 0 ) );
			return;
		}
		
		if( firstword == "whisper" )
		{
			// get the players name
			string name = ParseWord( p_data, 1 );
			string message = RemoveWord( RemoveWord( p_data, 0 ), 0 );
			
			Whisper( message, name );
			return;
		}
		
		if( firstword == "who" )
		{
			p.SendString( WhoList( BasicLib::LowerCase( ParseWord( p_data, 1 ) ) ) );
			return;
		}
		
		
		if( firstword == "look" || firstword == "l" )
		{
			p.SendString( PrintRoom( p.CurrentRoom() ) );
			return;
		}
		
		if( firstword == "north" || firstword == "n" )
		{
			Move( NORTH );
			return;
		}
		if( firstword == "east" || firstword == "e" )
		{
			Move( EAST );
			return;
		}
		if( firstword == "south" || firstword == "s" )
		{
			Move( SOUTH );
			return;
		}
		if( firstword == "west" || firstword == "w" )
		{
			Move( WEST );
			return;
		}
		
		if( firstword == "get" || firstword == "take" )
		{
			GetItem( RemoveWord( p_data, 0 ) );
			return;
		}
		
		if( firstword == "drop" )
		{
			DropItem( RemoveWord( p_data, 0 ) );
			return;
		}
		
		if( firstword == "attack" || firstword == "a" )
		{
			PlayerAttack( RemoveWord( p_data, 0 ) );
			return;
		}

		if( firstword == "talk" || firstword == "t" )
		{
			switch(p.CurrentRoom()->Type()){
				case PLAINROOM:
					p.SendString(bold + white + "Every thing is ordinary in this place.");
					break;
				case SAVEROOM:
					p.SendString(bold + white + "");
					break;
				case STORE:
					p.SendString(PrintProducts());
					break;
				case LOCKEDROOM:
					p.SendString(bold + white + "Every thing is ordinary in this place.");
					break;
				default:
					p.SendString(bold + white + "Every thing is ordinary in this place.");
					break;
			}
			return;
		}
		
		if( firstword == "buy" )
		{
			string feedback;
			if(p.CurrentRoom()->Type() == STORE){
				string name = RemoveWord( p_data, 0 );
				int id;
				if(Utils::stringToNumber<int>(id, name, std::dec)){
					feedback = buyProduct(id);
				}
				else{
					feedback = buyProduct(name);
				}
			}
			else
				feedback = bold + red + "There is no store in this place.";

			p.SendString(feedback);
			return;
		}

		if( firstword == "sell" )
		{
			if(p.CurrentRoom()->Type() == STORE)
				p.SendString(SellItem(RemoveWord(p_data,0)));
			else
				p.SendString(bold + red + "There is no store in this place.");
			return;
		}

		if( firstword == "inspect" )
		{
			p.SendString(PrintItemStats( RemoveWord(p_data,0) ));
		}

		if( firstword == "map" )
		{
			p.SendString(PrintMap());
			return;
		}

		if( firstword == "team" )
		{
			string name = RemoveWord( p_data, 0 );
			if(p.CreateTeam(name))
				p.SendString(bold + white + "Team " + name + " is created.");
			else
				p.SendString(bold + red + "Team creation is failed.");
			return;
		}

		if( firstword == "invite" )
		{
			string name = ParseWord( p_data, 1 );

			if(!m_player->joinedTeam)
				return;

			PlayerDatabase::iterator itr = PlayerDatabase::findactive( name );
			if( itr == PlayerDatabase::end() )
				return;
			else
				if(p.Invite(itr->ID()))
					p.SendString(bold + white + "Invitation is sent.");
				else
					p.SendString(bold + white + "Fail to Invitation.");

			return;
		}

		if( firstword == "leave" )
		{
			if(p.LeaveTeam())
				p.SendString(bold + white + "You have left the team.");
			else
				p.SendString(bold + red + "Fail to leave the team.");
			return;
		}

		if( firstword == "confirm" )
		{
			string name = RemoveWord( p_data, 0 );
			if(p.ConfirmTeam(name))
				p.SendString(bold + white + "You've joined the team " + name + ".");
			else
				p.SendString(bold + red + "Fail to join the team " + name + ".");
			return;
		}

		if( firstword == "tellteam" || firstword == "tt")
		{
			string messege = RemoveWord( p_data, 0 );
			TalkToTeam(messege);
			return;
		}

		if (firstword == "listteam" || firstword == "lt" )
		{
			if(p.joinedTeam)
				p.SendString(p.joinedTeam->ListMembers());
			else
				p.SendString(bold + red + "You do not have a team.");
			return;
		}

		if (firstword == "skill")
		{
			p.SendString(PrintSkills(p.Rank()));
			return;
		}

		if( firstword == "peek" && (p.Rank()==ARCHER || p.Rank()==ADMIN))
		{
			string direction = BasicLib::LowerCase( ParseWord( p_data, 1 ) );
			room r;
			if(direction == "north" || direction == "n")
				r = p.CurrentRoom()->Adjacent(NORTH);
			else if(direction == "east" || direction == "e")
				r = p.CurrentRoom()->Adjacent(EAST);
			else if(direction == "south" || direction == "s")
				r = p.CurrentRoom()->Adjacent(SOUTH);
			else if(direction == "west" || direction == "w")
				r = p.CurrentRoom()->Adjacent(WEST);
			else{
				p.SendString(bold + red + "The direction is illegal");
				return;
			}

			if(r == 0){
				p.SendString(bold + red + "There is no room at " + direction);
			}
			else{
				Skills::peekEnemies(m_player, r);
			}
			return;
		}

		if( firstword == "jump" && (p.Rank()==MAGE || p.Rank()==ADMIN))
		{
			string roomIdString = ParseWord(p_data,1);
			int id;
			if(Utils::stringToNumber<int>(id, roomIdString, std::dec)){
				Skills::jump(m_player, id);
			}
			else{
				p.SendString(bold + red + "The room id is invalid.");
			}
		}

		if( firstword == "push" && (p.Rank()==WARRIOR || p.Rank()==ADMIN))
		{
			Skills::push( m_player, p.CurrentRoom(), RemoveWord(p_data,0) );
			return;
		}

		// ------------------------------------------------------------------------
		//  ADMIN access commands
		// ------------------------------------------------------------------------
		if( firstword == "kick" && p.Rank() >= ADMIN )
		{
			// find a player to kick
			PlayerDatabase::iterator itr = 
            PlayerDatabase::findloggedin( ParseWord( p_data, 1 ) );
			
			if( itr == PlayerDatabase::end() )
			{
				p.SendString( red + bold + "Player could not be found." );
				return;
			}
			
			if( itr->Rank() > p.Rank() )
			{
				p.SendString( red + bold + "You can't kick that player!" );
				return;
			}
            
			itr->Conn()->Close();
			LogoutMessage( itr->Name() + " has been kicked by " + 
						  p.Name() + "!!!" );
			
			return;
		}
		
		if( firstword == "announce" && p.Rank() >= ADMIN )
		{
			Announce( RemoveWord( p_data, 0 ) );
			return;
		}
		
		if( firstword == "changerank" && p.Rank() >= ADMIN )
		{
			string name = ParseWord( p_data, 1 );
			
			PlayerDatabase::iterator itr = PlayerDatabase::find( name );
			
			if( itr == PlayerDatabase::end() )
			{
				p.SendString( red + bold + "Error: Could not find user " +
							 name );
				return;
			}
			
			PlayerRank rank = GetRank( ParseWord( p_data, 2 ) );
			
			itr->Rank() = rank;
			SendGame( green + bold + itr->Name() + 
					 "'s rank has been changed to: " +
					 GetRankString( rank ) );
			return;
		}
		
		
		if( firstword == "reload" && p.Rank() >= ADMIN )
		{
			string db = BasicLib::LowerCase( ParseWord( p_data, 1 ) );
			
			if( db == "items" )
			{
				ItemDatabase::Load();
				p.SendString( bold + cyan + "Item Database Reloaded!" );
			}
			else if( db == "player" )
			{
				string user = ParseWord( p_data, 2 );
				PlayerDatabase::iterator itr = PlayerDatabase::findfull( user );
				
				if( itr == PlayerDatabase::end() )
				{
					p.SendString( bold + red + "Invalid Player Name" );
				}
				else
				{
					bool a = itr->Active();
					if( a )     itr->Conn()->Handler()->Leave();
					PlayerDatabase::LoadPlayer( itr->Name() );
					if( a )     itr->Conn()->Handler()->Enter();
					
					p.SendString( bold + cyan + "Player " + 
								 itr->Name() + " Reloaded!" );
				}
			}
			else if( db == "rooms" )
			{
				RoomDatabase::LoadTemplates();
				p.SendString( bold + cyan + "Room Template Database Reloaded!" );
			}
			else if( db == "stores" )
			{
				StoreDatabase::Load();
				p.SendString( bold + cyan + "Store Database Reloaded!" );
			}
			else if( db == "enemies" )
			{
				EnemyTemplateDatabase::Load();
				p.SendString( bold + cyan + "Enemy Database Reloaded!" );
			}
			else
			{
				p.SendString( bold + red + "Invalid Database Name" );
			}
			return;
		}
		
		if( firstword == "shutdown" && p.Rank() >= ADMIN )
		{
			Announce( "SYSTEM IS SHUTTING DOWN" );
			Game::Running() = false;
			return;
		}
		
		
		// ------------------------------------------------------------------------
		//  Command not recognized, send to room
		// ------------------------------------------------------------------------
		SendRoom( bold + p.Name() + " says: " + dim + p_data, p.CurrentRoom() );
	}
	
	
	// ------------------------------------------------------------------------
	//  This notifies the handler that there is a new connection
	// ------------------------------------------------------------------------
	void Game::Enter()
	{
		USERLOG.Log(  
					GetIPString( m_connection->GetRemoteAddress() ) + 
					" - User " + m_player->Name() + 
					" entering Game state." );
		
		m_lastcommand = "";
		
		Player& p = *m_player;
		p.CurrentRoom()->AddPlayer( p.ID() );
		p.Active() = true;
		p.LoggedIn() = true;
		
		SendGame( bold + green + p.Name() + " has entered the realm." );
		
		if( p.Newbie() )
			GotoTrain();
		else
			p.SendString( PrintRoom( p.CurrentRoom() ) );
	}
	
	void Game::Leave()
	{
		USERLOG.Log(  
					GetIPString( m_connection->GetRemoteAddress() ) + 
					" - User " + m_player->Name() + 
					" leaving Game state." );
		
		// remove the player from his room
		m_player->CurrentRoom()->RemovePlayer( m_player );
		m_player->Active() = false;
		
		// log out the player from the database if the connection has been closed
		if( m_connection->Closed() )
		{
			PlayerDatabase::Logout( m_player );
		}
	}
	
	
	// ------------------------------------------------------------------------
	//  This notifies the handler that a connection has unexpectedly hung up.
	// ------------------------------------------------------------------------
	void Game::Hungup()
	{
		USERLOG.Log(  
					GetIPString( m_connection->GetRemoteAddress() ) + 
					" - User " + m_player->Name() + 
					" hung up." );
		
		Player& p = *m_player;
		LogoutMessage( p.Name() + " has suddenly disappeared from the realm." );
	}
	
	// ------------------------------------------------------------------------
	//  This notifies the handler that a connection is being kicked due to 
	//  flooding the server.
	// ------------------------------------------------------------------------
	void Game::Flooded()
	{
		USERLOG.Log(  
					GetIPString( m_connection->GetRemoteAddress() ) + 
					" - User " + m_player->Name() + 
					" flooded." );
		
		Player& p = *m_player;
		LogoutMessage( p.Name() + " has been kicked for flooding!" );
	}
	
	// ------------------------------------------------------------------------
	//  Sends a string to everyone connected.
	// ------------------------------------------------------------------------
	void Game::SendGlobal( const string& p_str )
	{
		operate_on_if( PlayerDatabase::begin(),
					  PlayerDatabase::end(),
					  playersend( p_str ),
					  playerloggedin() );
	}
	
	// ------------------------------------------------------------------------
	//  Sends a string to everyone "within the game"
	// ------------------------------------------------------------------------
	void Game::SendGame( const std::string& p_str )
	{
		operate_on_if( PlayerDatabase::begin(),
					  PlayerDatabase::end(),
					  playersend( p_str ),
					  playeractive() );
	}
	
	// ------------------------------------------------------------------------
	//  Sends a logout message
	// ------------------------------------------------------------------------
	void Game::LogoutMessage( const string& p_reason )
	{
		SendGame( SocketLib::red + SocketLib::bold + p_reason );
	}
	
	// ------------------------------------------------------------------------
	//  Sends a system announcement
	// ------------------------------------------------------------------------
	void Game::Announce( const string& p_announcement )
	{
		SendGlobal( SocketLib::cyan + SocketLib::bold + 
				   "System Announcement: " + p_announcement );
	}
	
	// ------------------------------------------------------------------------
	//  Sends a whisper string to the requested player
	// ------------------------------------------------------------------------
	void Game::Whisper( std::string p_str, std::string p_player )
	{
		// find the player
		PlayerDatabase::iterator itr = PlayerDatabase::findactive( p_player );
		
		// if no match was found
		if( itr == PlayerDatabase::end() )
		{
			m_player->SendString( red + bold + "Error, cannot find user." );
		}
		else
		{
			itr->SendString( yellow + m_player->Name() + " whispers to you: " + 
							reset + p_str );
			
			m_player->SendString( yellow + "You whisper to " + itr->Name() + 
								 ": " + reset + p_str );
		}
	}

	void Game::TalkToTeam(const string& messege){
		m_player->joinedTeam->Say(messege, m_player);
	}
	
	// ------------------------------------------------------------------------
	//  Functor that generates a who-listing for a single player
	// ------------------------------------------------------------------------
	struct wholist
	{
		string str;
		
		void operator() ( Player& p )
		{
			str += " " + tostring( p.Name(), 17 ) + "| ";
			str += tostring( p.Level(), 10 ) + "| ";
			
			
			if( p.Active() )
				str += green + "Online  " + white;
			else if( p.LoggedIn() )
				str += yellow + "Inactive" + white;
			else
				str += red + "Offline " + white;
			
			str += " | ";
			switch( p.Rank() )
			{
				case REGULAR:   str += white;   break;
				case ADMIN:     str += green;   break;
				default:       str += yellow;  break;
			}
			
			str += GetRankString( p.Rank() );
			
			str += white + "\r\n";
		}
	};
	
	// ------------------------------------------------------------------------
	//  This prints up the who-list for the realm.
	// ------------------------------------------------------------------------
	string Game::WhoList( const string& p_who )
	{
		using namespace BasicLib;
		
		string str = white + bold +
        "--------------------------------------------------------------------------------\r\n" + 
        " Name             | Level     | Activity | Rank\r\n" + 
        "--------------------------------------------------------------------------------\r\n";
		
		wholist who;
		if( p_who == "all" )
		{
			who = BasicLib::operate_on_if( 
										  PlayerDatabase::begin(),
										  PlayerDatabase::end(),
										  wholist(),
										  always<Player>() );
		}
		else
		{
			who = BasicLib::operate_on_if( 
										  PlayerDatabase::begin(),
										  PlayerDatabase::end(),
										  wholist(),
										  playerloggedin() );
		}
		
		str += who.str;
		
		
		str +=
        "--------------------------------------------------------------------------------";
		
		return str;
	}
	
	// ------------------------------------------------------------------------
	//  Prints out a help listing based on a user's rank.
	// ------------------------------------------------------------------------
	string Game::PrintHelp( PlayerRank p_rank )
	{
		static string help = white + bold + 
        "--------------------------------- Command List ---------------------------------\r\n" + 
        " /                            - Repeats your last command exactly.\r\n" +
        " chat <msg>                   - Sends message to everyone in the game\r\n" +
        " experience                   - Shows your experience statistics\r\n" +
        " help                         - Shows this menu\r\n" +
        " inventory                    - Shows a list of your items\r\n" + 
        " quit                         - Allows you to leave the realm.\r\n" + 
        " remove <'weapon'/'armor'>    - Removes your weapon or armor\r\n" + // added
        " stats                        - Shows all of your statistics\r\n" + 
		" train                        - Go to train using StatPoints\r\n" +
        " time                         - Shows the current system time.\r\n" +
        " use <item>                   - Use an item in your inventory\r\n" +
        " whisper <who> <msg>          - Sends message to one person\r\n" +
        " who                          - Shows a list of everyone online\r\n" +
        " who all                      - Shows a list of everyone\r\n" +
        " look                         - Shows you the contents of a room\r\n" + // added
        " north/east/south/west        - Moves in a direction\r\n" +          // added
        " get/drop <item>              - Picks up or drops an item on the ground\r\n" + // added
        " attack <enemy>               - Attacks an enemy\r\n" +               //added
		" talk                         - Talks to NPC's in the room\r\n" +
		" buy <item name / id>         - Buys an item from the store\r\n" +
		" sell <item name>             - Sells an item to the store\r\n" +
		" inspect <item name>          - Shows stats of an item\r\n" +
		" map                          - Shows the map\r\n" +
		" team <team name>             - Creates a team with a team name\r\n" +
		" invite <who>                 - Invites a player to join your team\r\n" +
		" confirm <team name>          - Confirms to a team invitation\r\n" +
		" leave                        - Leaves the team\r\n" +
		" tellteam <msg>               - Talks to your teammates\r\n" +
		" listteam                     - Shows a list of your team members\r\n" +
		" skill                        - Shows your skill list\r\n";
				
		static string admin = green + bold +
        "-------------------------------- Admin Commands --------------------------------\r\n" + 
        " kick <who>                   - Kicks a user from the realm\r\n"+
        " announce <msg>               - Makes a global system announcement\r\n" + 
        " changerank <who> <rank>      - Changes the rank of a player\r\n" + 
        " reload <db>                  - Reloads the requested database\r\n" + 
        " shutdown                     - Shuts the server down\r\n";
		
		static string end = white + bold + 
        "--------------------------------------------------------------------------------";
		
		
		if( p_rank==REGULAR || p_rank==WARRIOR || p_rank==MAGE || p_rank==ARCHER)
			return help + end;
		else if( p_rank == ADMIN )
			return help + admin + end;
		else return "ERROR";
		
	}
	
	// ------------------------------------------------------------------------
	//  This prints up the stats of the player
	// ------------------------------------------------------------------------
	string Game::PrintStats()
	{
		using namespace BasicLib;
		Player& p = *m_player;
		
		return (bold + white + 
			"--------------------------------- Your Stats ----------------------------------\r\n" +
			"Name:        " + p.Name() + "\r\n" +
			"Rank:        " + GetRankString(p.Rank()) + "\r\n" +
			"HP/Max:      " + tostring(p.HitPoints()) + "/" + tostring(p.GetAttr(MAXHITPOINTS)) + "     (" + tostring((int)(100*p.HitPoints()/p.GetAttr(MAXHITPOINTS))) + "%)\r\n" +
			PrintExperience() +
			"Strength:    " + tostring(p.GetAttr(STRENGTH), 5) + "Accuracy:       " + tostring(p.GetAttr(ACCURACY)) + "\r\n" +
			"Health:      " + tostring(p.GetAttr(HEALTH), 5)   + "Dodging:        " + tostring(p.GetAttr(DODGING)) + "\r\n" +
			"Agility:     " + tostring(p.GetAttr(AGILITY), 5)  + "Strike Damage:  " + tostring(p.GetAttr(STRIKEDAMAGE)) + "\r\n" +
			"StatPoints:  " + tostring(p.StatPoints(), 5)      + "Damage Absorb:  " + tostring(p.GetAttr(DAMAGEABSORB)) + "\r\n" +
			"-------------------------------------------------------------------------------\r\n"
			);
	}
	
	// ------------------------------------------------------------------------
	//  This prints up the experience of the player
	// ------------------------------------------------------------------------
	string Game::PrintExperience()
	{
		using namespace BasicLib;
		Player& p = *m_player;

		int needForNextLevel = p.NeedForNextLevel();
		double percentage = 100 * p.Experience() / needForNextLevel;
		
		return (bold + white +
			"Level:       " + tostring(p.Level()) + "\r\n" +
			"Experience:  " + tostring(p.Experience()) + "/" + tostring(needForNextLevel) + "(" + tostring(floor(percentage)) + "%)\r\n"
			);
	}
	
	// ------------------------------------------------------------------------
	//  This prints up the inventory-list of the player
	// ------------------------------------------------------------------------
	string Game::PrintInventory()
	{
		using namespace BasicLib;
		Player& p = *m_player;
		
		// Inventory
		string itemlist = white + bold +
        "-------------------------------- Your Inventory --------------------------------\r\n" +
        " Items:  ";
		
		for( int i = 0; i < PLAYERITEMS; i++ )
		{
			if( p.GetItem( i ) != 0 )
			{
				itemlist += p.GetItem( i )->Name() + ", ";
			}
		}
		
		// chop off the extraneous comma, and add a newline.
		itemlist.erase( itemlist.size() - 2, 2 );
		itemlist += "\r\n";
		
		// Weapon/Armor
		itemlist += " Weapon: ";
		if( p.Weapon() == 0 )
			itemlist += "NONE!";
		else
			itemlist += p.Weapon()->Name();
		
		itemlist += "\r\n Armor:  ";
		if( p.Armor() == 0 )
			itemlist += "NONE!";
		else
			itemlist += p.Armor()->Name();
		
		// Money
		itemlist += "\r\n Money:  $" + tostring( p.Money() );
		
		itemlist +=
        "\r\n--------------------------------------------------------------------------------";
		
		return itemlist;
	}

	// ------------------------------------------------------------------------
	//  Prints out the skill list on a user's rank.
	// ------------------------------------------------------------------------
	string Game::PrintSkills( PlayerRank p_rank )
	{
		static string regular = white + red +
			"You don't have any learnt skills.";

		static string warrior = white + bold + 
			"-------------------------------- Warrior Commands -----------------------------\r\n" + 
			" push <enemy>                 - Push the enemy to the next room\r\n";

		static string mage = white + bold +
			"-------------------------------- Mage Commands --------------------------------\r\n" + 
			" jump <room id>               - Jumps to the specific room\r\n";
		
		static string archer = white + bold +
			"-------------------------------- Archer Commands ------------------------------\r\n" + 
			" peek <north/east/south/west> - Peeks enemy information in the next room\r\n";

		static string end = white + bold + 
			"-------------------------------------------------------------------------------\r\n";
		
		
		if(p_rank==REGULAR)
			return regular;
		else if(p_rank==WARRIOR)
			return warrior + end;
		else if(p_rank==MAGE)
			return mage + end;
		else if(p_rank==ARCHER)
			return archer + end;
		else if( p_rank == ADMIN )
			return warrior + mage + archer + end;
		else return "ERROR";
	}
	
	// ------------------------------------------------------------------------
	//  This prints up the products of the store
	// ------------------------------------------------------------------------
	string Game::PrintProducts(){
		Store& s = *(store)m_player->CurrentRoom()->ID();
		int id = 1;
		string productList = white + bold +
			"--------------------------------------------------------------------------------\r\n" + 
			" ID    | Price      | Name\r\n" +
			"--------------------------------------------------------------------------------\r\n";

		for(Store::iterator itr = s.begin(); itr!= s.end(); ++itr, ++id){
			productList += " " + tostring(id, 5) + " | " + tostring(itr->first->Price() + itr->second, 10) + " | " + itr->first->Name() + "\r\n";
		}
		
		productList +=
			"\r\n--------------------------------------------------------------------------------";

		return productList;
	}
	
	string Game::buyProduct(int id){
		string feedback = bold + "";
//		if(m_player->m_items >= PLAYERITEMS)
//			return false;
		Store& s = *(store)m_player->CurrentRoom()->ID();
		id -= 1;
		if(id>=0 && id<s.ProductList().size()){
			item i = s.ProductList().at(id).first;
			int price = i->Price() + s.ProductList().at(id).second;
			if(price > m_player->Money())
				feedback += red + "You don't have enough money.";
			else{
				if(m_player->PickUpItem(i)){
					feedback += white + "You've bought " + i->Name() + ".";
					m_player->Money() -= price;
				}
				else
					feedback += red + "Your inventory is full.";
			}
		}
		else
			feedback += red + "The id entered is invalid.";

		return feedback;
	}

	string Game::buyProduct(const string& name){
		string feedback = bold + "";
		Store& s = *(store)m_player->CurrentRoom()->ID();
		Store::iterator itr = s.find(name);
		
		if(itr != s.end()){
			item i = itr->first;
			int price = i->Price() + itr->second;
			if(price > m_player->Money())
				feedback += red + "You don't have enough money.";
			else{
				if(m_player->PickUpItem(i)){
					feedback += white + "You've bought " + i->Name() + ".";
					m_player->Money() -= price;
				}
				else
					feedback += red + "Your inventory is full.";
			}
		}
		else
			feedback += red + "There is no such an item.";

		return feedback;
	}
	
	string Game::SellItem(const string& itemName){
		string feedback = bold + "";
		int id = m_player->GetItemIndex(itemName);
		if(id==-1)
			feedback += red + "You don't have such an item.";
		else{
			int price = m_player->GetItem(id)->Price() * 0.4;
			m_player->Money() += price;
			m_player->DropItem(id);
			feedback += white + "You've sold " + itemName + " for $" + tostring(price) + ".";
		}
		return feedback;
	}

	string Game::PrintMap(){
		string feedback = white + 
			"-----------------------------------------------------\r\n" +
			"  METROPOLIS                               MOUNTAIN  \r\n" +
            "                                                     \r\n" +
			"              009                                    \r\n" +
			"               |                                     \r\n" +
			"              008-010-011                            \r\n" +
			"               |       |                             \r\n" +
			"              007     012                            \r\n" +
			"               |       |                             \r\n" +
			"              006     013-----014-015-016            \r\n" +
			"               |       |               |             \r\n" +
			"              005-004-003             017-018-019    \r\n" +
			"               |       |                       |     \r\n" +
			"              052     002                     020    \r\n" +
			"               |       |                       |     \r\n" +
			"          050-051     001                     021    \r\n" +
			"           |           |                       |     \r\n" +
			"          049         048             024-023-022    \r\n" +
			"           |           |               |             \r\n" +
			"  044-043-042-045-046-047             025            \r\n" +
			"           |                           |             \r\n" +
			"           |                           |             \r\n" +
			"           |                           |             \r\n" +
			"           |                           |             \r\n" +
			"           |                           |             \r\n" +
			"          041                 028-----026            \r\n" +
			"           |                           |             \r\n" +
			"          040     037-038-030-029-----027            \r\n" +
			"           |       |   |   |                         \r\n" +
			"          039-036-035-034 031                        \r\n" +
			"                       |   |                         \r\n" +
			"                      033-032                        \r\n" +
            "                                                     \r\n" +
			"              FOREST                                 \r\n" +
			"-----------------------------------------------------\r\n";

		return feedback;
	}

	string Game::PrintItemStats(const string& name){
		ItemDatabase::iterator itr = ItemDatabase::begin();
		string feedback;
		for(; itr!=ItemDatabase::end(); ++itr){
			if(itr->Name() == name){
				return (bold + white +
					"--------------------------------- Item Stats ----------------------------------\r\n" +
					"Name         : " + tostring(itr->Name()) + "\r\n" +
					"Type         : " + GetItemTypeString(itr->Type()) + "\r\n" +
					"Min          : " + tostring(itr->Min()) + "\r\n" +
					"Max          : " + tostring(itr->Max()) + "\r\n" +
					"Price        : " + tostring(itr->Price()) + "\r\n" +
					"Speed        : " + tostring(itr->Speed()) + "\r\n" +
					"Strength     : " + tostring(itr->GetAttr(STRENGTH), 5)  + "Health       : " + tostring(itr->GetAttr(HEALTH)) + "\r\n" +
					"Dodging      : " + tostring(itr->GetAttr(DODGING), 5)   + "MAXHITPOINTS : " + tostring(itr->GetAttr(MAXHITPOINTS)) + "\r\n" +
					"ACCURACY     : " + tostring(itr->GetAttr(ACCURACY), 5)  + "HPREGEN      : " + tostring(itr->GetAttr(HPREGEN)) + "\r\n" +
					"Agility      : " + tostring(itr->GetAttr(AGILITY), 5)   + "Strike Damage: " + tostring(itr->GetAttr(STRIKEDAMAGE)) + "\r\n" +
					"Damage Absorb: " + tostring(itr->GetAttr(DAMAGEABSORB)) + "\r\n" +
					"-------------------------------------------------------------------------------\r\n"
				);
			}
		}
		return (bold + red + "There is no such an item.");
	}
	
	// ------------------------------------------------------------------------
	//  This finds and attempts to "use" an item in your inventory.
	// ------------------------------------------------------------------------
	bool Game::UseItem( const std::string& p_item )
	{
		Player& p = *m_player;
		
		int i = p.GetItemIndex( p_item );
		
		if( i == -1 )
		{
			p.SendString( red + bold + "Could not find that item!" );
			return false;
		}
		
		Item& itm = *p.GetItem( i );
		
		
		switch( itm.Type() )
		{
			case WEAPON:
				p.UseWeapon( i );	// set player's weapon id to i
				SendRoom( green + bold + p.Name() + " arms a " + itm.Name(), 
						 p.CurrentRoom() );
				return true;
				
			case ARMOR:
				p.UseArmor( i );	// set player's armor id to i
				SendRoom( green + bold + p.Name() + " puts on a " + itm.Name(), 
						 p.CurrentRoom() );
				return true;
				
			case HEALING:
				p.AddBonuses( itm.ID() );	// set player's attribute
				p.AddHitpoints( BasicLib::RandomInt( itm.Min(), itm.Max() ) );
				p.DropItem( i );	// heal player, then drop item
				SendRoom( green + bold + p.Name() + " uses a " + itm.Name(), 
						 p.CurrentRoom() );
			case SPECIAL:
				RoomDatabase::iterator itr = RoomDatabase::begin();
				for(;itr != RoomDatabase::end(); ++itr){
					if(itr->Type()==CENTRAL && itr->InArea()==p.CurrentRoom()->InArea()){
						Skills::jump(m_player, itr->ID());
						p.DropItem(i);
						SendRoom( green + bold + p.Name() + " uses a " + itm.Name(),
							p.CurrentRoom() );
						return true;
					}
				}
				p.SendString(bold + red + "You cannot jump in this area.");
				return false;
				
				return true;
		}
		return false;
	}
	
	// ------------------------------------------------------------------------
	//  This removes your weapon or your armor
	// ------------------------------------------------------------------------
	bool Game::RemoveItem( std::string p_item )
	{
		Player& p = *m_player;
		
		p_item = BasicLib::LowerCase( p_item );
		
		if( p_item == "weapon" && p.Weapon() != 0 )
		{
			SendRoom( green + bold + p.Name() + " puts away a " + 
					 p.Weapon()->Name(), p.CurrentRoom() );
			p.RemoveWeapon();
			return true;
		}
		
		if( p_item == "armor" && p.Armor() != 0 )
		{
			SendRoom( green + bold + p.Name() + " takes off a " + 
					 p.Armor()->Name(), p.CurrentRoom() );
			p.RemoveArmor();
			return true;
		}
		
		p.SendString( red + bold + "Could not Remove item!" );
		return false;
	}
	
	
	string Game::PrintRoom( room p_room )
	{
		string desc = "\r\n" + bold + white + GetAreaString(p_room->InArea()) + " - " + p_room->Name() + "\r\n";
		string temp;
		int count;
		
		desc += bold + magenta + p_room->Description() + "\r\n";
		desc += bold + green + "exits: ";
		
		for( int d = 0; d < NUMDIRECTIONS; d++ )
		{					//¡ôNORTH, EAST, SOUTH, WEST
			if( p_room->Adjacent( d ) != 0 )
				desc += DIRECTIONSTRINGS[d] + "  "; // "north", "east", "south", "west"

			if(p_room->Adjacent(d) == -1){

			}
		}
		desc += "\r\n";
		
		
		// ---------------------------------
		// ITEMS
		// ---------------------------------
		temp = bold + yellow + "You see: ";
		count = 0;
		if( p_room->Money() > 0 )
		{
			count++;
			temp += "$" + tostring( p_room->Money() ) + ", ";
		}
		
		std::list<item>::iterator itemitr = p_room->Items().begin();
		while( itemitr != p_room->Items().end() )
		{
			count++;
			temp += (*itemitr)->Name() + ", ";
			++itemitr;
		}
		
		if( count > 0 )
		{
			// chop off the trailing ", "
			temp.erase( temp.size() - 2, 2 );
			// add a newline
			desc += temp + "\r\n";
		}
		
		// ---------------------------------
		// PEOPLE
		// ---------------------------------
		temp = bold + cyan + "People: ";
		count = 0;
		std::list<player>::iterator playeritr = p_room->Players().begin();
		while( playeritr != p_room->Players().end() )
		{
			temp += (*playeritr)->Name() + ", ";
			count++;
			++playeritr;
		}
		
		if( count > 0 )
		{
			temp.erase( temp.size() - 2, 2 );
			desc += temp + "\r\n";
		}
		
		// ---------------------------------
		// ENEMIES
		// ---------------------------------
		temp = bold + red + "Enemies: ";
		count = 0;
		std::list<enemy>::iterator enemyitr = p_room->Enemies().begin();
		while( enemyitr != p_room->Enemies().end() )
		{
			temp += (*enemyitr)->Name() + ", ";
			count++;
			++enemyitr;
		}
		
		if( count > 0 )
		{
			temp.erase( temp.size() - 2, 2 );
			desc += temp + "\r\n";
		}
		
		
		return desc;
	}
	
	void Game::SendRoom( string p_text, room p_room )
	{
		std::for_each( p_room->Players().begin(),
					  p_room->Players().end(),
					  playersend( p_text ) );
	}
	
	
	
	void Game::Move( int p_direction ) // n, e, w, s
	{
		Player& p = *m_player;
		room next = p.CurrentRoom()->Adjacent( p_direction );
		room previous = p.CurrentRoom();
		
		if( next == 0 )
		{
			SendRoom( red + p.Name() + " bumps into the wall to the " +
					 DIRECTIONSTRINGS[p_direction] + "!!!",
					 p.CurrentRoom() );
			return;
		}
		
		previous->RemovePlayer( p.ID() ); // remove player from the previous room
		
		SendRoom( green + p.Name() + " leaves to the " + 
				 DIRECTIONSTRINGS[p_direction] + ".",
				 previous );
		SendRoom( green + p.Name() + " enters from the " + 
				 DIRECTIONSTRINGS[OppositeDirection(p_direction)] + ".",
				 next );
		p.SendString( green + "You walk " + DIRECTIONSTRINGS[p_direction] + "." );
		
		p.CurrentRoom() = next;	// set player's new room object
		next->AddPlayer( p.ID() );	// add player to next room object
		
		p.SendString( PrintRoom( next ) );	// look the next room
	}
	
	
	
	void Game::GetItem( string p_item )
	{
		Player& p = *m_player;
		
		if( p_item[0] == '$' )
		{
			// clear off the '$', and convert the result into a number.
			p_item.erase( 0, 1 );
			money m = BasicLib::totype<money>( p_item );
			
			// make sure there's enough money in the room
			if( m > p.CurrentRoom()->Money() )	// if money to get > money in room
			{
				p.SendString( red + bold + "There isn't that much here!" );
			}
			else
			{
				p.Money() += m;	// add m to player's money
				p.CurrentRoom()->Money() -= m;	// reduce room's money
				SendRoom( cyan + bold + p.Name() + " picks up $" + 
						 tostring( m ) + ".", p.CurrentRoom() );
			}
			return;
		}
		
		item i = p.CurrentRoom()->FindItem( p_item );	// find item in the room
		
		if( i == 0 )	// if item not found
		{
			p.SendString( red + bold + "You don't see that here!" );
			return;
		}
		
		if( !p.PickUpItem( i ) )	// if player has not enough inventory (not succeed picking up)
		{
			p.SendString( red + bold + "You can't carry that much!" );
			return;
		}
		
		p.CurrentRoom()->RemoveItem( i );	// remove item from the room
		SendRoom( cyan + bold + p.Name() + " picks up " + i->Name() + ".", 
				 p.CurrentRoom() );
	}
	
	
	void Game::DropItem( string p_item )
	{
		Player& p = *m_player;
		
		if( p_item[0] == '$' )
		{
			// clear off the '$', and convert the result into a number.
			p_item.erase( 0, 1 );
			money m = BasicLib::totype<money>( p_item );
			
			// make sure there's enough money in the inventory
			if( m > p.Money() )	// if money to drop > money you have
			{
				p.SendString( red + bold + "You don't have that much!" );
			}
			else
			{
				p.Money() -= m;	// deduct m from player's money
				p.CurrentRoom()->Money() += m;	// add money to current room
				SendRoom( cyan + bold + p.Name() + " drops $" + 
						 tostring( m ) + ".", p.CurrentRoom() );
			}
			return;
		}
		
		int i = p.GetItemIndex( p_item );
		
		if( i == -1 )
		{
			p.SendString( red + bold + "You don't have that!" );
			return;
		}
		
		SendRoom( cyan + bold + p.Name() + " drops " +
				 p.GetItem( i )->Name() + ".", p.CurrentRoom() );
		p.CurrentRoom()->AddItem( p.GetItem( i ) );	// add item to the room
		p.DropItem( i );	// remove item i from player
	}
	
	
	void Game::GotoTrain()
	{
		Player& p = *m_player;
		p.Active() = false;
		p.Conn()->AddHandler( new Train( *m_connection, p.ID() ) );
		LogoutMessage( p.Name() + " leaves to edit stats" );
	}
	
	void Game::EnemyAttack( enemy p_enemy )	// enemy starts attacking player
	{
		Enemy& e = *p_enemy;
		room r = e.CurrentRoom();	// r is current room
		
		std::list<player>::iterator itr = r->Players().begin();
		
		std::advance( itr, BasicLib::RandomInt( 0, r->Players().size() - 1 ) );
		
		Player& p = **itr;
		
		sint64 now = Game::GetTimer().GetMS();
		
		int damage;
		if( e.Weapon() == 0 )	// if enemy has no weapon
		{
			damage = BasicLib::RandomInt( 1, 3 );
			e.NextAttackTime() = now + seconds( 1 );	// next attack time = next 1s
		}
		else
		{
			damage = BasicLib::RandomInt( e.Weapon()->Min(), e.Weapon()->Max() );
			e.NextAttackTime() = now + seconds( e.Weapon()->Speed() );	// next attack time = new + weapon speed
		}
		
		if( BasicLib::RandomInt( 0, 99 ) >= e.Accuracy() - p.GetAttr( DODGING ) )	// calculate if enemy misses the player
		{
			Game::SendRoom( white + e.Name() + " swings at " + p.Name() + 
						   " but misses!", e.CurrentRoom() );
			return;	// return if miss
		}
		
		damage += e.StrikeDamage();	// damage += enemy strike - player
		damage -= p.GetAttr( DAMAGEABSORB );
		
		if( damage < 1 )	// let minimun damage = 1
			damage = 1;
		
		p.AddHitpoints( -damage );
		
		Game::SendRoom( red + e.Name() + " hits " + p.Name() + " for " + 
					   tostring( damage ) + " damage!", e.CurrentRoom() );
		
		
		if( p.HitPoints() <= 0 )	// if player hitpoint < 0
			PlayerKilled( p.ID() );	// then die
	}
	
	void Game::PlayerKilled( player p_player )
	{
		Player& p = *p_player;
		
		Game::SendRoom( red + bold + p.Name() + " has died!", p.CurrentRoom() );
		
		// drop the money
		money m = p.Money() / 10;	// drop 10% of player's money
		if( m > 0 )
		{
			p.CurrentRoom()->Money() += m;	// drop money to the room
			p.Money() -= m;	// deduct m from player's money
			Game::SendRoom( cyan + "$" + tostring( m ) + 
						   " drops to the ground.", p.CurrentRoom() );
		}
		
		// drop an item
		if( p.Items() > 0 )
		{
			int index = -1;
			
			// loop through random numbers until you hit a valid item.
			while( p.GetItem( index = RandomInt( 0, PLAYERITEMS - 1 ) ) == 0 );
			item i = p.GetItem( index );
			p.CurrentRoom()->AddItem( i );	// add item i to the room
			p.DropItem( index );	// drop item i from the player
			
			Game::SendRoom( cyan + i->Name() + " drops to the ground.", 
						   p.CurrentRoom() );
		}
		
		// subtract 10% experience
		int exp = p.Experience() / 10;
		p.Experience() -= exp;	// deduct 10% exp from the player
		
		// remove the player from the room and transport him to room 1.
		p.CurrentRoom()->RemovePlayer( p_player );
		p.CurrentRoom() = 1;
		p.CurrentRoom()->AddPlayer( p_player );
		
		// set the hitpoints to 70% 
		p.SetHitpoints( (int)(p.GetAttr( MAXHITPOINTS ) * 0.7) );	// 70% from player's hp
		p.SendString( white + bold + "You have died, but have been ressurected in " +
					 p.CurrentRoom()->Name() );
		p.SendString( red + bold + "You have lost " + tostring( exp ) + " experience!" );
		Game::SendRoom( white + bold + p.Name() + " appears out of nowhere!!" , p.CurrentRoom() );
	}
	
	
	
	void Game::PlayerAttack( const string& p_enemy )
	{
		Player& p = *m_player;
		sint64 now = Game::GetTimer().GetMS();
		
		if( now < p.NextAttackTime() )	// if next attack time is reached
		{
			p.SendString( red + bold + "You can't attack yet!" );
			return;
		}
		
		
		enemy ptr = p.CurrentRoom()->FindEnemy( p_enemy );
		
		if( ptr == 0 )
		{
			p.SendString( red + bold + "You don't see that enemy here!" );
			return;
		}
		
		Enemy& e = *ptr;
		
		int damage;
		if( p.Weapon() == 0 )	// if no weapon
		{
			damage = BasicLib::RandomInt( 1, 3 );	// damage = 1 or 2 or 3
			p.NextAttackTime() = now + seconds( 1 );
		}
		else
		{
			damage = BasicLib::RandomInt( p.Weapon()->Min(), p.Weapon()->Max() );	// damage = weapon min to max
			p.NextAttackTime() = now + seconds( p.Weapon()->Speed() );
		}
		
		if( BasicLib::RandomInt( 0, 99 ) >= p.GetAttr( ACCURACY ) - e.Dodging() )	// calculate if player misses the enemy
		{
			SendRoom( white + p.Name() + " swings at " + e.Name() + 
					 " but misses!", p.CurrentRoom() );
			return;
		}
		
		damage += p.GetAttr( STRIKEDAMAGE );
		damage -= e.DamageAbsorb();
		
		if( damage < 1 )
			damage = 1;
		
		e.HitPoints() -= damage;
		
		SendRoom( red + p.Name() + " hits " + e.Name() + " for " + 
				 tostring( damage ) + " damage!", p.CurrentRoom() );
		
		
		if( e.HitPoints() <= 0 )
			EnemyKilled( e.ID(), m_player );
	}
	
	
	void Game::EnemyKilled( enemy p_enemy, player p_player )
	{
		Enemy& e = *p_enemy;
		
		SendRoom( cyan + bold + e.Name() + " has died!", e.CurrentRoom() );
		
		
		// drop the money
		money m = BasicLib::RandomInt( e.MoneyMin(), e.MoneyMax() );
		if( m > 0 )
		{
			e.CurrentRoom()->Money() += m;	// add money to the room
			SendRoom( cyan + "$" + tostring( m ) + 
					 " drops to the ground.", e.CurrentRoom() );
		}
		
		// drop all the items
		std::list<loot>::iterator itr = e.LootList().begin();
		while( itr != e.LootList().end() )	// for every loot
		{
			if( BasicLib::RandomInt( 0, 99 ) < itr->second )	// if random < loot's probability
			{
				e.CurrentRoom()->AddItem( itr->first );	// add the item to the room
				SendRoom( cyan + (itr->first)->Name() +		// display item's name
						 " drops to the ground.", e.CurrentRoom() );
			}
			++itr;
		}
		
		Player& p = *p_player;

		if(p.joinedTeam){
			p.joinedTeam->ShareExp(p.ID(), e.Experience()*0.2);

			p.Experience() += e.Experience() * 0.6;	// add experience to the player
			p.SendString( cyan + bold + "You gain " + tostring( e.Experience() * 0.6 ) + " experience." );

			p.RaiseLevel();
		}
		else{
			// add experience to the player who killed it
			p.Experience() += e.Experience();	// add experience to the player
			p.SendString( cyan + bold + "You gain " + tostring( e.Experience() ) + " experience." );

			p.RaiseLevel();
		}
		
		// remove the enemy from the game
		EnemyDatabase::Delete( p_enemy );
	}

/*
	bool Game::ConfirmTeam(const string& teamName){
		for(std::list<Team*>::iterator itr = m_player->invitedTeam.begin(); itr != m_player->invitedTeam.end(); ++itr){
			if(teamName==(*itr)->Name()){
				return (*itr)->AddMember(m_player.m_id);
			}
		}
		return false;
	}
	bool Game::LeaveTeam(){
		if(m_player->joinedTeam){
			if(m_player->joinedTeam->RemoveMember(m_player.m_id)){
				m_player->joinedTeam = NULL;
				return true;
			}
		}
		return false;
	}
	bool Game::InviteTeam(const string& p_player){
		if(!m_player->joinedTeam)
			return false;

		PlayerDatabase::iterator itr = PlayerDatabase::findactive( p_player );

		if( itr == PlayerDatabase::end() )
		{
			return false;
		}
		else
		{
			itr->invitedTeam.push_back(m_player->joinedTeam);
			itr->SendString( yellow + m_player->Name() + " invites you to join the " + m_player->joinedTeam->Name() + " team." );
			return true;
		}
	}
	bool Game::CreateTeam(const string& teamName){
		m_player->joinedTeam = new Team(teamName, m_player.m_id);
		return true;
	}*/

}   // end namespace SimpleMUD
