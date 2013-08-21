#ifndef MISSIONS
#define MISSIONS

#include "Player.h"

namespace SimpleMUD
{
	class Missions{
	public:
		static void room4_blacksmith(Player p);
/*
			0 - request not yet received
			1 - on mission
			2 - just completed
			3 - reward claimed
*/
		static void room28_gatekeeper(Player p);
/*
			0 - request not yet received
			1 - on mission
			2 - boss defeated
			3 - skull chain possessed
			4 - chain returned
*/
		static void room19_boss(Player p);
	};
}

#endif