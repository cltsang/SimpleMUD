// MUD Programming
// Ron Penton
// (C)2003
// Logon.cpp - This class is the logon handler for SimpleMUD.
// 
// 

#include "Logon.h"
#include "PlayerDatabase.h"
#include "BasicLib/BasicLib.h"

using namespace SocketLib;

namespace SimpleMUD
{

// ------------------------------------------------------------------------
//  This handles incomming commands. Anything passed into this function
//  is assumed to be a complete command from a client.
// ------------------------------------------------------------------------
void Logon::Handle( string p_data )
{
	if(m_errors >= 6){
		m_connection->Close();
		return;
	}
	switch(m_state){
		case NEWCONNECTION:
			if(BasicLib::LowerCase(p_data) == "new"){
				m_state = NEWUSER;
				m_connection->Protocol().SendString(*m_connection, bold +
					white + "Please enter your new login name: " +
					reset);
				return;
			}
			else{
			    PlayerDatabase::iterator itr = PlayerDatabase::findfull( p_data );
//				std::map<entityid, Player>::iterator itr = PlayerDatabase::findfull(p_data);
				if(itr==PlayerDatabase::end()) {
					m_errors++;
					m_connection->Protocol().SendString( *m_connection, bold +
						red + "Sorry, user " +
						green + p_data + red + " does not exist.\r\n" +
						white + "Please enter your name, or \"new\" if you are new user: "+
						reset);
					return;
				}
				else if(itr->LoggedIn()){
					m_errors++;
					m_connection->Protocol().SendString( *m_connection, bold +
						red + "Sorry, user " +
						green + p_data + red + " is logged in.\r\n" +
						white + "Please enter your name, or \"new\" if you are new user: " +
						reset);
					return;
				}
				else{
					m_state = ENTERPASS;
					m_name = p_data;
					m_pass = itr->Password();
					m_connection->Protocol().SendString( *m_connection, bold +
						white + "Please enter the password: " +
						reset + bwhite);
					return;
				}
			}
			break;
		case NEWUSER:
			// Check if p_data is in Database
			// If yes m_errors++;
			if(PlayerDatabase::hasfull(p_data)){
				m_errors++;
				m_state = NEWCONNECTION;
				m_connection->Protocol().SendString( *m_connection, bold +
					red + bold + "Sorry, the name " + green + p_data +
					red + " is already used.\r\n" +
					white + "Please enter your name, or \"new\" if you are new user: " +
					reset);
				return;
			}
			// Check if AcceptibleName(p_data)
			// If no m_errors++;
			// If the new name is valid, m_state = ENTERNEWPASS
			else if(!AcceptibleName(p_data)){
				m_errors++;
				m_state = NEWCONNECTION;
				m_connection->Protocol().SendString( *m_connection, bold +
					red + bold + "Sorry, the name " + green + p_data +
					red + " contains invalid characters.\r\n" +
					white + "Please enter your name, or \"new\" if you are new user: " +
					reset);
				return;
			}
			else{
				m_state = ENTERNEWPASS;
				m_name = p_data;
				m_connection->Protocol().SendString( *m_connection, reset + bold +
					white + "Please enter your new password: " +
                    reset + bwhite);
				return;
			}
			break;
		case ENTERNEWPASS:
			m_connection->Protocol().SendString(*m_connection, reset + bold +
					white + "Thank you! You are now entering the realm...\r\n" +
					reset);
			{
				Player *p = new Player();
				p->ID() = PlayerDatabase::LastID() + 1;
				p->Name() = m_name;
				p->Password() = p_data;
				PlayerDatabase::AddPlayer(*p);
				delete p;
			}
			GotoGame(true);
			return;
			break;
		case ENTERPASS:
//			std::cout << "m_name=" << m_name << " m_pass=" << m_pass << " p_data=" << p_data << std::endl;
			if( m_pass == p_data ) {
					m_connection->Protocol().SendString( *m_connection, reset + bold +
						white + "Thank you! You are now entering the realm...\r\n" +
						reset );
					GotoGame(false);
					return;
			}
			else {
				m_errors++;
				m_state = NEWCONNECTION;
				m_connection->Protocol().SendString( *m_connection, reset + bold +
						red + "The password is incorrect.\r\n" +
						white + "Please enter your name, or \"new\" if you are new user: " +
						reset);
				return;
			}
			break;
	}
}


// ------------------------------------------------------------------------
//  This notifies the handler that there is a new connection
// ------------------------------------------------------------------------
void Logon::Enter()
{
	m_connection->Protocol().SendString(*m_connection, bold +
		green + "Welcome to my game world! \r\n" +
		white + "Please enter your name, or \"new\" if you are new user: "
		+reset);
}

// ------------------------------------------------------------------------
//  This changes the game state so that the player enters the game.
// ------------------------------------------------------------------------
void Logon::GotoGame( bool p_newbie )
{
    Player& p = *PlayerDatabase::findfull( m_name );
    
    // log off the user if he's already connected.            
    if( p.LoggedIn() )
    {
        p.Conn()->Close();
        p.Conn()->Handler()->Hungup();
        p.Conn()->ClearHandlers();
    }

    p.Newbie() = p_newbie;

    // record the users new connection
    p.Conn() = m_connection;

    // go to the game.
    p.Conn()->RemoveHandler();
	p.Conn()->AddHandler(new Game(*p.Conn(), p.ID()));
}

// ------------------------------------------------------------------------
//  This checks if a user name is acceptible.
// ------------------------------------------------------------------------
bool Logon::AcceptibleName( const string& p_name )
{
    static string inv = " \"'~!@#$%^&*+/\\[]{}<>()=.,?;:";

    // must not contain any invalid characters
    if(  p_name.find_first_of( inv ) != string::npos )
        return false;

    // must be less than 17 chars and more than 2
    if( p_name.size() > 16 || p_name.size() < 3 )
        return false;

    // must start with an alphabetical character
    if( !isalpha( p_name[0] ) )
        return false;

    if( p_name == "new" )
        return false;

    return true;
}



}   // end namespace SimpleMUD

