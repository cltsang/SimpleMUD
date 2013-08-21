#ifndef TEAM
#define TEAM

#include <vector>
#include "Player.h"
#include "Room.h"

class Player;

namespace SimpleMUD
{
	
const int TEAMMEMBERS = 6;

class Team{
public:

	Team(const string& teamName, player creator);
	int NumMembers();
	string Name();
	bool Exists();
	Team* AddMember(player p);
	bool RemoveMember(player p);
	void Say(const string & messege, player p);
	string ListMembers();
	void ShareExp(player p, int exp);

private:
	bool exists;
	std::vector<player> team;
	int memberCount;
	string name;
};


}

#endif