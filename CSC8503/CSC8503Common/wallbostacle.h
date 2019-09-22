#pragma once
#include "GameObject.h"
#include "RenderObject.h"
#include "StateMachine.h"
using namespace NCL::CSC8503;
enum WallDirection {
	WX,WY,WZ
};
struct DataTable{
	float TimeIdle;
	float CurrentTime;
	float FrameTime;
	float LimitDistance;
	float CurrentDistance;
	float Speed;
	bool StopBeingIdle;
	Vector3 OriginalPosition;
	WallDirection direction;
	
};

class WallObstacle : public GameObject {
public:
	WallObstacle();
	~WallObstacle();
	void update(float dt);
	void setOriginalPosition(Vector3 position) { Data.OriginalPosition = position; }
	void setTimeIdle(float time) { Data.TimeIdle = time; }
	void setSpeed(float newSpeed) { Data.Speed = newSpeed; }
	void setLimitDistance(float Distance) { Data.LimitDistance = Distance; }
private:


	
	DataTable Data;
	
	StateMachine * WallStateMachine;
};