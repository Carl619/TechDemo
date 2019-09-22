#include "hole.h"
#include "CollisionDetection.h"
#include "gamestatemachine.h"
#include "player.h"
using namespace NCL;

bool Hole::checkGoalHit(std::vector<Player * > players)
{
	for(Player* player: players){
		CollisionDetection::CollisionInfo info;
		if (CollisionDetection::ObjectIntersection(this, player, info))
			return true;
	}
	return false;
}

void Hole::update(float dt)
{
	if (checkGoalHit(playersVector)) {
		if (GameStateMachine::getInstance()->getActiveState() == GameState::ONLINECLIENT)
			GameStateMachine::getInstance()->replace(GameState::ONLINECHECKRESULTS);
		else
			GameStateMachine::getInstance()->replace(GameState::RESULTMENU);
	}
}
