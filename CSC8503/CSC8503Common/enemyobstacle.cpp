#include "enemyobstacle.h"
#include "State.h"
#include "StateTransition.h"
#include "CollisionDetection.h"
#include "player.h"
using namespace NCL;

EnemyObstacle::EnemyObstacle() : GameObject()
{
	EnemyStateMachine = new StateMachine();
	Data = DataEneTable();
	Data.LimitDistance = 200;
	Data.Speed = 20;
	StateFunc Move = [](void * data) {
		DataEneTable * realData = (DataEneTable *)data;
		int MoveX = rand() % 100;
		int MoveY = rand() % 100;
		Vector3 newPosition = Vector3();
		newPosition = realData->ActualPosition;
		if (MoveX >  50)
			newPosition.x += realData->Speed * realData->FrameTime;
		else if (MoveX <=50)
			newPosition.x -= realData->Speed * realData->FrameTime;
		if (MoveY >50)
			newPosition.z += realData->Speed * realData->FrameTime;
		else if (MoveY <= 50)
			newPosition.z -= realData->Speed * realData->FrameTime;
		realData->CurrentDistance = sqrtf(pow(newPosition.x - realData->Center.x, 2) + pow(newPosition.y- realData->Center.y, 2) + pow(newPosition.z- realData->Center.z, 2));
		if (realData->CurrentDistance < realData->LimitDistance)
			realData->ActualPosition = newPosition;
		std::cout << "In State Move!" << std::endl;
	};

	GenericState * stateA = new GenericState(Move, (void *)& Data);

	EnemyStateMachine->AddState(stateA);
}

EnemyObstacle::~EnemyObstacle()
{
}

void EnemyObstacle::update(float dt)
{
	Data.FrameTime = dt;
	Data.ActualPosition = transform.GetWorldPosition();
	EnemyStateMachine->Update();
	CollisionDetection::CollisionInfo info;
		if (CollisionDetection::ObjectIntersection(this, Objective, info)) {
			Objective->HaveToRespawn();
		}
		transform.SetWorldPosition(Data.ActualPosition);
}
