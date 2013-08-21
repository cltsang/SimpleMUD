#include "Team.h"

namespace SimpleMUD{

	Team::Team(const string& teamName, player creator){
		exists = true;
		memberCount = 0;
		team.resize(TEAMMEMBERS+1);
		name = teamName;
		AddMember(creator);
	}

	int Team::NumMembers(){
		return memberCount;
	}

	string Team::Name(){
		return name;
	}
	
	bool Team::Exists(){
		return exists;
	}

	Team* Team::AddMember(player p){
		if(memberCount <= TEAMMEMBERS){
			memberCount += 1;
			team.at(memberCount) = p;
//			p->joinedTeam = (Team*)this;
			Say("Here I come!", p);
			return this;
		}
		else
			return NULL;
	}

	bool Team::RemoveMember(player p){
		for(std::vector<player>::iterator itr=team.begin(); itr!=team.end(); ++itr){
			if(itr->m_id == p.m_id){
				team.erase(itr);
				memberCount -= 1;

				if(memberCount==0){
					exists = false;
				}
				else
					Say("I'm leaving the team.", p);

				return true;
			}
		}
		return false;
	}

	void Team::Say(const string & messege, player p){
		for(int i=1; i<memberCount; ++i){
			if(team[i]->ID() != p->ID())
				team[i]->SendString(SocketLib::yellow + "Messege from team member " + p->Name() + ": " + SocketLib::reset + messege);
		}
	}

	string Team::ListMembers(){
		string feedback = "------------------------------\r\n";
		for(int i=1; i<=memberCount; ++i){
			feedback += team[i]->Name() + "\r\n";
		}
		feedback += "------------------------------\r\n";

		return feedback;
	}

	void Team::ShareExp(player p, int exp){
		for(int i=1; i<=memberCount; ++i){
			if(team[i]->ID()!=p->ID() && team[i]->CurrentRoom()->ID()==p->CurrentRoom()->ID()){
				team[i]->Experience() += exp;
				team[i]->SendString( SocketLib::cyan + SocketLib::bold + "You gain " + tostring( exp ) + " experience." );
				team[i]->RaiseLevel();
			}
		}
	}

}