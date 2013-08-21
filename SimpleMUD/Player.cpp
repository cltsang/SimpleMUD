// MUD Programming
// Ron Penton
// (C)2003
// Player.cpp - The class defining Players in the SimpleMUD
// 
// 

#include <cmath>
#include <string>
#include <fstream>
#include <vector>

#include "SimpleMUDLogs.h"
#include "Player.h"
#include "Item.h"
#include "Game.h"

namespace SimpleMUD
{



Player::Player()
{
    m_pass = "UNDEFINED";
    m_rank = REGULAR;

    m_connection = 0;
    m_loggedin = false;
    m_active = false;
    m_newbie = true;

    m_experience = 0;
    m_level = 1;
    m_room = 1;
    m_money = 0;

    m_nextattacktime = 0;

    m_baseattributes[STRENGTH] = 1;
    m_baseattributes[HEALTH]   = 1;
    m_baseattributes[AGILITY]  = 1;

    m_items = 0;
    m_weapon = -1;
    m_armor = -1;

    m_statpoints = 18;

    RecalculateStats();
    m_hitpoints = GetAttr( MAXHITPOINTS );

	joinedTeam = NULL;
}

bool Player::AbleToAttack()
{
	sint64 now = Game::GetTimer().GetMS();
	if( now < NextAttackTime() )	// if next attack time is reached
	{
		return false;
	}
	else
		return true;
}

void Player::RaiseLevel(){
	bool raised = false;
	do{
		if(m_experience >= NeedForNextLevel()){
			m_experience -= NeedForLevel(Level());
			Level() += 1;
			raised = true;
			SendString(SocketLib::cyan + SocketLib::bold + "Congratulations! Your level is raised to " + tostring(m_level) + "!" );

			m_statpoints += 5;
			m_money += NeedForLevel(Level()-1);
			SendString(SocketLib::cyan + SocketLib::bold + "You've gained 5 stat points and " + tostring(NeedForLevel(m_level-1)) + " dollars!");
		}
		else
			raised = false;
	}while(raised);

	if(m_level==2){
		PlayerRank rank = PlayerRank(BasicLib::RandomInt(1,3));

		m_rank = rank;
	}
}

int Player::NeedForNextLevel()
{
    // how much more experience to the next level
    return NeedForLevel( m_level + 1 ) - m_experience;
}


bool Player::Train()
{
    if( NeedForNextLevel() <= 0 )
    {
        m_statpoints += 2;
        m_baseattributes[MAXHITPOINTS] += m_level;
        m_level++;
        RecalculateStats();
        return true;
    }
    return false;
}


void Player::RecalculateStats()
{
    m_attributes[MAXHITPOINTS] = 
        10 + ( (int)(m_level * ( GetAttr( HEALTH ) / 1.5 )) );

    m_attributes[HPREGEN] = 
        ( GetAttr( HEALTH ) / 5 ) + m_level;

    m_attributes[ACCURACY] = GetAttr( AGILITY ) * 3;
    m_attributes[DODGING] = GetAttr( AGILITY ) * 3;
    m_attributes[DAMAGEABSORB] = GetAttr( STRENGTH ) / 5;
    m_attributes[STRIKEDAMAGE] = GetAttr( STRENGTH ) / 5;

    // make sure the hitpoints don't overflow if your max goes down
    if( m_hitpoints > GetAttr( MAXHITPOINTS ) )
        m_hitpoints = GetAttr( MAXHITPOINTS );

    if( Weapon() != 0 )
        AddDynamicBonuses( Weapon() );
    if( Armor() != 0 )
        AddDynamicBonuses( Armor() );
}

void Player::AddDynamicBonuses( item p_item )
{
    if( p_item == 0 )
        return;

    Item& i = *p_item;

    for( int x = 0; x < NUMATTRIBUTES; x++ )
        m_attributes[x] += i.GetAttr( x );
}

void Player::SetBaseAttr( int p_attr, int p_val )
{
    m_baseattributes[p_attr] = p_val;
    RecalculateStats();
}


void Player::AddToBaseAttr( int p_attr, int p_val )
{
    m_baseattributes[p_attr] += p_val;
    RecalculateStats();
}

void Player::AddHitpoints( int p_hitpoints )
{
    SetHitpoints( m_hitpoints + p_hitpoints );
}

void Player::SetHitpoints( int p_hitpoints )
{
    m_hitpoints = p_hitpoints;

    if( m_hitpoints < 0 )
        m_hitpoints = 0;
    if( m_hitpoints > GetAttr( MAXHITPOINTS ) )
        m_hitpoints = GetAttr( MAXHITPOINTS );
}


bool Player::PickUpItem( item p_item )
{
    if( m_items < MaxItems() )
    {
        // find the first open index to put the item in.
        item* itr = m_inventory;
        while( *itr != 0 )
            ++itr;

        // insert the item
        *itr = p_item;
        m_items++;

        return true;
    }
    return false;
}


bool Player::DropItem( int p_index )
{
    if( m_inventory[p_index] != 0 )
    {
        // remove the weapon or armor if needed
        if( m_weapon == p_index )
            RemoveWeapon();
        if( m_armor == p_index )
            RemoveArmor();

        m_inventory[p_index] = 0;
        m_items--;

        return true;
    }
    return false;
}

void Player::AddBonuses( item p_item )
{
    if( p_item == 0 )
        return;

    Item& itm = *p_item;
    for( int i = 0; i < NUMATTRIBUTES; i++ )
        m_baseattributes[i] += itm.GetAttr( i );

    RecalculateStats();
}


void Player::RemoveWeapon()
{
    m_weapon = -1;
    RecalculateStats();
}


void Player::RemoveArmor()
{
    m_armor = -1;
    RecalculateStats();
}


void Player::UseWeapon( int p_index )
{
    RemoveWeapon();
    m_weapon = p_index;
    RecalculateStats();
}


void Player::UseArmor( int p_index )
{
    RemoveArmor();
    m_armor = p_index;
    RecalculateStats();
}


// ------------------------------------------------------------------------
//  This gets the index of an item within the players inventory given a 
//  name.
// ------------------------------------------------------------------------
int Player::GetItemIndex( const std::string& p_name )
{
    item* i = double_find_if( m_inventory, 
                              m_inventory + MaxItems(),
                              matchentityfull( p_name ),
                              matchentity( p_name ) );

    if( i == m_inventory + MaxItems() )
        return -1;
    return (int)(i - m_inventory);
}


// ------------------------------------------------------------------------
//  This sends a string to the players connection.
// ------------------------------------------------------------------------
void Player::SendString( const std::string& p_string )
{
    using namespace SocketLib;

    if( Conn() == 0 )
    {
        ERRORLOG.Log( "Trying to send string to player " +
                      Name() + " but player is not connected." );
        return;
    }

    // send the string, newline included.
    Conn()->Protocol().SendString( *Conn(), p_string + newline );

    if( Active() )
    {
        PrintStatbar();
    }
}

// ------------------------------------------------------------------------
//  This prints up the players "statbar", ie: his hitpoints.
// ------------------------------------------------------------------------
void Player::PrintStatbar( bool p_update )
{
    using namespace SocketLib;
    using namespace BasicLib;

    // if this is a statusbar update and the user is currently typing something,
    // then do nothing.
    if( p_update && Conn()->Protocol().Buffered() > 0 )
        return;

    string statbar = white + bold + "[";

    int ratio = 100 * HitPoints() / GetAttr( MAXHITPOINTS );

    // color code your hitpoints so that they are red if low,
    // yellow if medium, and green if high.
    if( ratio < 33 )
        statbar += red;
    else if( ratio < 67 )
        statbar += yellow;
    else 
        statbar += green;

    statbar += tostring( HitPoints() ) + white + "/" + 
                tostring( GetAttr( MAXHITPOINTS ) ) + "] ";

    Conn()->Protocol().SendString( *Conn(), clearline + "\r" + statbar + reset );
}



// --------------------------------------------------------------------
//  Inserts an item in text form into a stream
// --------------------------------------------------------------------
ostream& operator<<( ostream& p_stream, const Player& p )
{
    p_stream << "[NAME]           " << p.m_name << "\n";
    p_stream << "[PASS]           " << p.m_pass << "\n";
    p_stream << "[RANK]           " << GetRankString( p.m_rank ) << "\n";
    p_stream << "[STATPOINTS]     " << p.m_statpoints << "\n";
    p_stream << "[EXPERIENCE]     " << p.m_experience << "\n";
    p_stream << "[LEVEL]          " << p.m_level << "\n";
    p_stream << "[ROOM]           " << p.m_room << "\n";
    p_stream << "[MONEY]          " << p.m_money << "\n";
    p_stream << "[HITPOINTS]      " << p.m_hitpoints << "\n";
    p_stream << "[NEXTATTACKTIME] "; insert( p_stream, p.m_nextattacktime );
    p_stream << "\n";
    p_stream << p.m_baseattributes;

    p_stream << "[INVENTORY]      ";
    for( int i = 0; i < p.MaxItems(); i++ )
    {
        p_stream << p.m_inventory[i] << " ";
    }
    p_stream << "\n";


    p_stream << "[WEAPON]         " << p.m_weapon << "\n";
    p_stream << "[ARMOR]          " << p.m_armor << "\n";

    return p_stream;
}


// --------------------------------------------------------------------
//  Extracts an item in text form from a stream
// --------------------------------------------------------------------
istream& operator>>( istream& p_stream, Player& p )
{
    std::string temp;
    p_stream >> temp >> std::ws;
    std::getline( p_stream, p.m_name );
    p_stream >> temp >> p.m_pass;
    p_stream >> temp >> temp;
    p.m_rank = GetRank( temp );
    p_stream >> temp >> p.m_statpoints;
    p_stream >> temp >> p.m_experience;
    p_stream >> temp >> p.m_level;
    p_stream >> temp >> p.m_room;
    p_stream >> temp >> p.m_money;
    p_stream >> temp >> p.m_hitpoints;
    p_stream >> temp; extract( p_stream, p.m_nextattacktime );
    p_stream >> p.m_baseattributes;

    p_stream >> temp;
    p.m_items = 0;
    for( int i = 0; i < p.MaxItems(); i++ )
    {
        p_stream >> p.m_inventory[i];
        if( p.m_inventory[i] != 0 )   
            p.m_items++;
    }

    p_stream >> temp >> p.m_weapon;
    p_stream >> temp >> p.m_armor;

    p.RecalculateStats();

    return p_stream;
}


bool Player::CreateTeam(const string& teamName){
	joinedTeam = new Team(teamName, m_id);
	return true;
}
bool Player::ConfirmTeam(const string& teamName){
	for(std::list<Team*>::iterator itr=invitedTeam.begin(); itr!=invitedTeam.end(); ++itr){
		if((*itr)->Exists() && teamName==(*itr)->Name()){
			joinedTeam = (*itr)->AddMember(m_id);
			return true;
		}
	}
	return false;
}
bool Player::LeaveTeam(){
	if(joinedTeam){
		if(joinedTeam->RemoveMember(m_id)){
			joinedTeam = NULL;
			return true;
		}
	}
	return false;
}
bool Player::Invite(player p){
	if(!joinedTeam)
		return false;
		p->invitedTeam.push_back(joinedTeam);
	p->SendString( SocketLib::yellow + Name() + " invites you to join the " + joinedTeam->Name() + " team." );
	return true;
}




}   // end namespace SimpleMUD
