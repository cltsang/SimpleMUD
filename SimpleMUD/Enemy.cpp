// MUD Programming
// Ron Penton
// (C)2003
// Enemy.cpp - The class defining Enemies in the SimpleMUD
// 
// 

#include "Enemy.h"
#include "Player.h"
#include "Item.h"
#include "BasicLib/BasicLib.h"

namespace SimpleMUD
{



EnemyTemplate::EnemyTemplate()
{
    m_hitpoints = 0;
    m_accuracy = 0;
    m_dodging = 0;
    m_strikedamage = 0;
    m_damageabsorb = 0;
    m_experience = 0;
    m_weapon = 0;
    m_moneymin = 0;
    m_moneymax = 0;
}


// --------------------------------------------------------------------
//  reads an enemy template from a stream
// --------------------------------------------------------------------
istream& operator>>( istream& p_stream, EnemyTemplate& t )
{
    std::string temp;

    p_stream >> temp >> std::ws;  std::getline( p_stream, t.m_name );
    p_stream >> temp >> t.m_hitpoints;
    p_stream >> temp >> t.m_accuracy;
    p_stream >> temp >> t.m_dodging;
    p_stream >> temp >> t.m_strikedamage;
    p_stream >> temp >> t.m_damageabsorb;
    p_stream >> temp >> t.m_experience;
    p_stream >> temp >> t.m_weapon;
    p_stream >> temp >> t.m_moneymin;
    p_stream >> temp >> t.m_moneymax;

    t.m_loot.clear();
    while( extract( p_stream, temp ) != "[ENDLOOT]" )
    {
        entityid id;
        int chance;
        p_stream >> id >> chance;
        t.m_loot.push_back( loot( id, chance ) );
    }

    return p_stream;
}


Enemy::Enemy()
{
    m_template = 0;
    m_hitpoints = 0;
    m_room = 0;
    m_nextattacktime = 0;
}

void Enemy::LoadTemplate( enemytemplate p_template )
{
    m_template = p_template;
    m_hitpoints = p_template->m_hitpoints;
}


std::string& Enemy::Name()  { return m_template->Name(); }
int Enemy::Accuracy()       { return m_template->m_accuracy; }
int Enemy::Dodging()        { return m_template->m_dodging; }
int Enemy::StrikeDamage()   { return m_template->m_strikedamage; }
int Enemy::DamageAbsorb()   { return m_template->m_damageabsorb; }
int Enemy::Experience()     { return m_template->m_experience; }
item Enemy::Weapon()        { return m_template->m_weapon; }
money Enemy::MoneyMin()     { return m_template->m_moneymin; }
money Enemy::MoneyMax()     { return m_template->m_moneymax; }
list<loot>& Enemy::LootList() { return m_template->m_loot; }



// --------------------------------------------------------------------
//  writes an enemy instance to a stream
// --------------------------------------------------------------------
ostream& operator<<( ostream& p_stream, const Enemy& t )
{
    p_stream << "[TEMPLATEID]     " << t.m_template << "\n";
    p_stream << "[HITPOINTS]      " << t.m_hitpoints << "\n";
    p_stream << "[ROOM]           " << t.m_room << "\n";
    p_stream << "[NEXTATTACKTIME] "; insert( p_stream, t.m_nextattacktime ); 
    p_stream << "\n";

    return p_stream;
}


// --------------------------------------------------------------------
//  reads an enemy instance from a stream
// --------------------------------------------------------------------
istream& operator>>( istream& p_stream, Enemy& t )
{
    std::string temp;

    p_stream >> temp >> t.m_template;
    p_stream >> temp >> t.m_hitpoints;
    p_stream >> temp >> t.m_room;
    p_stream >> temp;   extract( p_stream, t.m_nextattacktime );

    return p_stream;
}


}   // end namespace SimpleMUD
