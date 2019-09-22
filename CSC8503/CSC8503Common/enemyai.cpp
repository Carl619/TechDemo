#include "enemyai.h"
#include "player.h"
#include "State.h"
#include "StateTransition.h"
#include "CollisionDetection.h"
#include "Debug.h"


using namespace NCL;
EnemyAI::EnemyAI() : GameObject()
{
	EnemyStateMachine = new StateMachine();

	StateFunc Move = [](void * data) {
		EnemyAIDataTable * realData = (EnemyAIDataTable *)data;
		realData->Direction = Vector3(0, 0, 0);
		if (realData->PathNodes.size() > 0) {
			realData->GoingNode = realData->PathNodes[0];
			if (realData->GoingNode == realData->PreviousNode && realData->GoingNode != realData->Objectives[realData->currentObjective]->GetTransform().GetWorldPosition())
				realData->GoingNode = realData->PathNodes[1];
			realData->Direction = realData->GoingNode - realData->ActualPosition;
			realData->acceleration += realData->Speed * realData->FrameTime;
			if (realData->acceleration > realData->limitacceleration)
				realData->acceleration = realData->limitacceleration;
			realData->Direction *= (realData->acceleration * realData->FrameTime);
			if (realData->Direction.y <= 0)
				realData->Direction.y = 0;
			realData->CurrentTimeToChange += realData->FrameTime;
			realData->CurrentTime += realData->FrameTime;
			Vector3 DistanceVector = realData->GoingNode - realData->ActualPosition;
			realData->Distance = sqrtf(pow(DistanceVector.x, 2) + pow(DistanceVector.z, 2));
		}
		std::cout << "In State Move!" << std::endl;
	};

	StateFunc ChangeObjective = [](void * data) {
		EnemyAIDataTable * realData = (EnemyAIDataTable *)data;
		int index = rand() % realData->Objectives.size();
		realData->currentObjective = index;
		realData->changedObjective = true;
		realData->haveToChange = false;
		realData->CurrentTime = 3;
		std::cout << "In State Change!" << std::endl;
	};


	StateFunc Attack = [](void * data) {
		EnemyAIDataTable * realData = (EnemyAIDataTable *)data;
		realData->attacked = true;
		realData->canAttack = false;
		Vector3 forceDirection = Vector3(rand() % 100+1, rand() % 5, rand() % 100+1);
		realData->Objectives[realData->currentObjective]->GetPhysicsObject()->AddForce(forceDirection.Normalised() * realData->force);
		std::cout << "In State Attack!" << std::endl;
	};

	StateFunc Stopping = [](void * data) {
		EnemyAIDataTable * realData = (EnemyAIDataTable *)data;
		Vector3 direction;
		if (realData->PathNodes.size() > 0) {
			realData->breakacceleration += realData->Speed * realData->FrameTime;
			if (realData->breakacceleration > realData->limitbreakacceleration)
				realData->breakacceleration = realData->limitbreakacceleration;
			realData->Direction *= (realData->breakacceleration * realData->FrameTime);
			if (realData->Direction.y <= 0)
				realData->Direction.y = 0;
			realData->PreviousNode = Vector3(realData->GoingNode);
			realData->CurrentTimeToChange += realData->FrameTime;
			realData->acceleration = 0;
			realData->CurrentTime += realData->FrameTime;
			Vector3 DistanceVector = realData->GoingNode - realData->ActualPosition;
			realData->Distance = sqrtf(pow(DistanceVector.x, 2) + pow(DistanceVector.z, 2));
		}
		std::cout << "In State Stopping!" << std::endl;
	};

	GenericState * stateA = new GenericState(Move, (void *)& Data);
	GenericState * stateB = new GenericState(ChangeObjective, (void *)& Data);
	GenericState * stateC = new GenericState(Attack, (void *)& Data);
	GenericState * stateD = new GenericState(Stopping, (void *)& Data);
	EnemyStateMachine->AddState(stateA);
	EnemyStateMachine->AddState(stateD);
	EnemyStateMachine->AddState(stateC);
	EnemyStateMachine->AddState(stateB);



	GenericTransition <float &, float& >* transitionA =
		new GenericTransition <float &, float& >(
			GenericTransition <float &, float& >::EqualLessThanTransition,
			Data.Distance, Data.LimitDistance, stateA, stateD); // if greater than 10 , A to B

	GenericTransition <float &, float& >* transitionB =
		new GenericTransition <float &, float& >(
			GenericTransition <float &, float& >::GreaterThanTransition,
			Data.Distance, Data.LimitDistance, stateD, stateA); // if equals 0 , B to A


	GenericTransition <bool &, bool >* transitionC =
		new GenericTransition <bool &, bool >(
			GenericTransition <bool &, bool >::EqualsTransition,
			Data.canAttack, true, stateA, stateC); // if equals 0 , B to A

	GenericTransition <bool &, bool >* transitionD =
		new GenericTransition <bool &, bool >(
			GenericTransition <bool &, bool >::EqualsTransition,
			Data.canAttack, true, stateD, stateC); // if equals 0 , B to A

	GenericTransition <bool &, bool >* transitionF =
		new GenericTransition <bool &, bool >(
			GenericTransition <bool &, bool >::EqualsTransition,
			Data.attacked, true, stateC, stateA); // if equals 0 , B to A

	GenericTransition <bool &, bool >* transitionG =
		new GenericTransition <bool &, bool >(
			GenericTransition <bool &, bool >::EqualsTransition,
			Data.haveToChange, true, stateA, stateB); // if equals 0 , B to A

	GenericTransition <bool &, bool >* transitionH =
		new GenericTransition <bool &, bool >(
			GenericTransition <bool &, bool >::EqualsTransition,
			Data.haveToChange, true, stateD, stateB); // if equals 0 , B to A

	GenericTransition <bool &, bool >* transitionJ =
		new GenericTransition <bool &, bool >(
			GenericTransition <bool &, bool >::EqualsTransition,
			Data.changedObjective, true, stateB, stateA); // if equals 0 , B to A


	EnemyStateMachine->AddTransition(transitionA);
	EnemyStateMachine->AddTransition(transitionB);
	EnemyStateMachine->AddTransition(transitionC);
	EnemyStateMachine->AddTransition(transitionD);
	EnemyStateMachine->AddTransition(transitionF);
	EnemyStateMachine->AddTransition(transitionG);
	EnemyStateMachine->AddTransition(transitionH);
	EnemyStateMachine->AddTransition(transitionJ);

	Data.LimitDistance = 1;
	Data.limitacceleration = 200;
	Data.limitbreakacceleration = 100;
	Data.Speed = 100000;
	Data.CurrentTime = 0;
	Data.CurrentTimeToChange = 0;
	Data.WaitChange = 15;
	Data.WaitAttack = 2;
	Data.haveToChange = false;
	Data.PreviousNode = Vector3(-1,-1,-1);
	Data.currentObjective = 0;
	Data.force = 30;
	grid = new NavigationGrid("TestGrid1.txt");
}

EnemyAI::~EnemyAI()
{
}

void EnemyAI::update(float dt)
{
	Data.FrameTime = dt;
	Data.ActualPosition = GetTransform().GetWorldPosition();
	TestPathfinding();
	Data.PathNodes.push_back(Data.Objectives[Data.currentObjective]->GetTransform().GetWorldPosition());
	EnemyStateMachine->Update();
	GetPhysicsObject()->AddForce(Data.Direction);
	if (Data.CurrentTime > Data.WaitAttack) {
		CollisionDetection::CollisionInfo info;
		if (CollisionDetection::ObjectIntersection(this, Data.Objectives[Data.currentObjective], info)) {
			Data.canAttack = true;
			Data.CurrentTime = 0;
		}
	}
	if (Data.CurrentTimeToChange > Data.WaitChange) {
		Data.haveToChange = true;
		Data.CurrentTimeToChange = 0;
	}
	DisplayPathfinding();
	Data.attacked = false;
	Data.changedObjective = false;
}

void EnemyAI::TestPathfinding()
{
	Data.PathNodes.clear();


	NavigationPath outPath;

	Data.FoundObjective = grid->FindPath(Data.ActualPosition, Data.Objectives[Data.currentObjective]->GetTransform().GetWorldPosition(), outPath);

	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		Data.PathNodes.push_back(pos);

	}
}

void EnemyAI::DisplayPathfinding()
{
	for (int i = 1; i < Data.PathNodes.size(); ++i) {
		Vector3 a = Data.PathNodes[i - 1];
		Vector3 b = Data.PathNodes[i];

		Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));

	}
}