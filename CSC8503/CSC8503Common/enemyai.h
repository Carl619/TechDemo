#pragma once
#include "GameObject.h"
#include "RenderObject.h"
#include "StateMachine.h"
#include "NavigationGrid.h"

using namespace NCL::CSC8503;
class Player;

struct EnemyAIDataTable {
	float CurrentTime;
	float FrameTime;
	float LimitDistance;
	float Distance;
	bool canAttack;
	float Speed;
	float acceleration;
	float breakacceleration;
	float limitacceleration;
	float limitbreakacceleration;
	Vector3 ActualPosition;
	Vector3 GoingNode;
	Vector3 Direction;
	int currentObjective;
	bool changedObjective;
	bool attacked;
	bool haveToChange;
	float WaitAttack;
	float WaitChange;
	float CurrentTimeToChange;
	bool FoundObjective;
	Vector3 PreviousNode;
	std::vector<Vector3> PathNodes;
	std::vector<Player*> Objectives;
	float force;
};

class EnemyAI : public GameObject {
public:
	EnemyAI();
	~EnemyAI();
	void update(float dt);
	void addPlayer(Player * pla) { Data.Objectives.push_back(pla); }
private:
	void TestPathfinding();
	void DisplayPathfinding();
	EnemyAIDataTable Data;
	bool attacked;
	NavigationGrid * grid;
	StateMachine * EnemyStateMachine;

	
};