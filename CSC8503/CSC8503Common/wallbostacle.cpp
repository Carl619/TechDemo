#include "wallbostacle.h"
#include "State.h"
#include "StateTransition.h"


WallObstacle::WallObstacle() : GameObject()
{
	WallStateMachine = new StateMachine();

	StateFunc Idle = [](void * data) {
		DataTable * realData = (DataTable *)data;
		realData->CurrentTime += realData->FrameTime;
		if (realData->CurrentTime >= realData->TimeIdle)
			realData->StopBeingIdle = true;
		std::cout << "In State Idle!" << std::endl;
	};
	StateFunc Move = [](void * data) {
		DataTable * realData = (DataTable *)data;
		if (realData->StopBeingIdle) {
			realData->CurrentTime = 0;
			realData->StopBeingIdle = false;
		}
		realData->CurrentDistance += realData->Speed*realData->FrameTime;
		if (realData->CurrentDistance >= realData->LimitDistance)
			realData->CurrentDistance = realData->LimitDistance;
		std::cout << "In State Move!" << std::endl;
	};

	StateFunc Back = [](void * data) {
		DataTable * realData = (DataTable *)data;
		if (realData->StopBeingIdle) {
			realData->CurrentTime = 0;
			realData->StopBeingIdle = false;
		}
		realData->CurrentDistance -= realData->Speed*realData->FrameTime;
		if (realData->CurrentDistance <= 0)
			realData->CurrentDistance = 0;
		std::cout << "In State Back!" << std::endl;
	};

	GenericState * stateA = new GenericState(Idle, (void *)& Data);
	GenericState * stateB = new GenericState(Move, (void *)& Data);
	GenericState * stateC = new GenericState(Back, (void *)& Data);
	GenericState * stateD = new GenericState(Idle, (void *)& Data);
	WallStateMachine->AddState(stateD);
	WallStateMachine->AddState(stateC);
	WallStateMachine->AddState(stateB);
	WallStateMachine->AddState(stateA);



	GenericTransition <float &, float& >* transitionA =
		new GenericTransition <float &, float& >(
			GenericTransition <float &, float& >::EqualGreaterThanTransition,
			Data.CurrentTime, Data.TimeIdle, stateA, stateB); // if greater than 10 , A to B

	GenericTransition <float &, float& >* transitionB =
		new GenericTransition <float &, float& >(
			GenericTransition <float &, float& >::EqualGreaterThanTransition,
			Data.CurrentDistance, Data.LimitDistance, stateB, stateD); // if equals 0 , B to A


	GenericTransition <float &, float >* transitionC =
		new GenericTransition <float &, float >(
			GenericTransition <float &, float >::EqualLessThanTransition,
			Data.CurrentDistance, 0.0f, stateC, stateA); // if equals 0 , B to A

	GenericTransition <float &, float& >* transitionD =
		new GenericTransition <float &, float& >(
			GenericTransition <float &, float& >::EqualGreaterThanTransition,
			Data.CurrentTime, Data.TimeIdle, stateD, stateC); // if equals 0 , B to A

	WallStateMachine->AddTransition(transitionA);
	WallStateMachine->AddTransition(transitionB);

	WallStateMachine->AddTransition(transitionC);
	WallStateMachine->AddTransition(transitionD);

	Data.direction = WallDirection::WY;
	Data.LimitDistance = 100;
	Data.TimeIdle = 5;
	Data.Speed = 20;
	Data.StopBeingIdle = false;
	Data.CurrentDistance = 0;
	Data.CurrentTime = 0;

}

WallObstacle::~WallObstacle()
{
}

void WallObstacle::update(float dt)
{
	Data.FrameTime = dt;
	WallStateMachine->Update();

	if(Data.direction == WallDirection::WX)
	transform.SetWorldPosition(Vector3(Data.OriginalPosition.x + Data.CurrentDistance, Data.OriginalPosition.y , Data.OriginalPosition.z));
	else if (Data.direction == WallDirection::WY)
		transform.SetWorldPosition(Vector3(Data.OriginalPosition.x, Data.OriginalPosition.y + Data.CurrentDistance, Data.OriginalPosition.z));
	else if (Data.direction == WallDirection::WZ)
		transform.SetWorldPosition(Vector3(Data.OriginalPosition.x, Data.OriginalPosition.y, Data.OriginalPosition.z + Data.CurrentDistance));
}
