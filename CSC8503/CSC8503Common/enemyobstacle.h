#pragma once
#include "GameObject.h"
#include "RenderObject.h"
#include "StateMachine.h"
using namespace NCL::CSC8503;
class Player;
struct DataEneTable {
	float CurrentTime;
	float FrameTime;
	float LimitDistance;
	float CurrentDistance;
	float Speed;
	Vector3 Center;
	Vector3 ActualPosition;
};

class EnemyObstacle : public GameObject {
public:
	EnemyObstacle();
	~EnemyObstacle();
	void update(float dt);
	void setObjecive(Player * pla) { Objective = pla; }
	void setCenter(Vector3 center) { Data.Center = center; }
private:
	DataEneTable Data;
	bool attacked;
	StateMachine * EnemyStateMachine;
	
	Player * Objective;
};