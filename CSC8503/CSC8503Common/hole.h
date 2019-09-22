#pragma once
#include "GameObject.h"
#include "RenderObject.h"
using namespace NCL::CSC8503;
class Player;
class Hole : public GameObject {
public:
	Hole() : GameObject() {}
	~Hole() {

	}
	bool checkGoalHit(std::vector<Player *> players);
	void update(float dt);
	void setPlayers(std::vector<Player *> players) { playersVector = players; }
private:
	std::vector<Player *> playersVector;
};