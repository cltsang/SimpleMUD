#ifndef SKILLS
#define SKILLS

#include <string>
#include "Game.h"

using namespace SocketLib;
using namespace BasicLib;

namespace SimpleMUD
{

class Skills{
public:
	static void jump(player p, room r){
		if(!p->AbleToAttack())
		{
			p->SendString( red + bold + "You can't use skill yet!" );
			return;
		}
		
		p->NextAttackTime() = Game::GetTimer().GetMS() + BasicLib::seconds(10);

		p->CurrentRoom()->RemovePlayer( p->ID() );
		
		Game::SendRoom(green + p->Name() + " jumps to " + r->Name() + ".",
			p->CurrentRoom() );
		Game::SendRoom(green + p->Name() + " jumps in ",
			r);
		p->SendString( green + "You jump to " + r->Name() + "." );
		
		p->CurrentRoom() = r;	// set player's new room object
		r->AddPlayer( p->ID() );	// add player to next room object
		
		p->SendString( Game::PrintRoom( r ) );	// look the next room

	}

	static void peekEnemies(player p, room r){
		if(!p->AbleToAttack())
		{
			p->SendString( red + bold + "You can't use skill yet!" );
			return;
		}

		p->NextAttackTime() = Game::GetTimer().GetMS() + BasicLib::seconds(10);

		std::list<enemy>::iterator itr = r->Enemies().begin();
		string enemyList;
		for(;itr != r->Enemies().end(); ++itr){
			enemyList += (*itr)->Name() + "\r\n";
		}
		if(enemyList=="")
			p->SendString(bold + red + "There is no enemy here.");
		else
			p->SendString(bold + white + enemyList);
	}

	static void push(player p, room origin, const string& p_enemy){
		if(!p->AbleToAttack())
		{
			p->SendString( red + bold + "You can't use skill yet!" );
			return;
		}

		p->NextAttackTime() = Game::GetTimer().GetMS() + BasicLib::seconds(10);
		
		enemy ptr = p->CurrentRoom()->FindEnemy( p_enemy );
		
		if( ptr == 0 )
		{
			p->SendString( red + bold + "You don't see that enemy here!" );
			return;
		}
		
		Enemy& e = *ptr;

		origin->RemoveEnemy(e.ID());
		room next;
		int direction;
		bool valid = true;
		do{
			direction = BasicLib::RandomInt(0,3);
			next = origin->Adjacent(direction);
			if(next==0 || next->Type()==SAVEROOM || next->Type()==LOCKEDROOM)
				valid = false;
			else
				valid = true;
		}while(!valid);
		e.CurrentRoom() = next;
		next->AddEnemy(e.ID());

		p->SendString( bold + blue + e.Name() + " is pushed to " + GetDirectionString(Direction(direction)) + "!");
	}
	
};

}

#endif