#include "Missions.h"

namespace SimpleMUD
{
	void Missions::room4_blacksmith(Player p){
	}
	void Missions::room28_gatekeeper(Player p){
	}
	void Missions::room19_boss(Player p){
	}
}


/*
			switch(p.CurrentRoom()){
				case 4:
					Missions::room4_blacksmith(p);
					break;
				case 19:
					Missions::room19_boss(p);
					break;
				case 28:
					Missions::room28_gatekeeper(p);
					break;
				default:
					p.SendString(bold + red + "There is no NPC in this place");
					break;
			}
*/