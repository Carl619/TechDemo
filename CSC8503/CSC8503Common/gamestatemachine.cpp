#include "gamestatemachine.h"
#include "State.h"
#include "StateTransition.h"

using namespace NCL::CSC8503;

GameStateMachine * NCL::CSC8503::GameStateMachine::getInstance()
{
	{
		static GameStateMachine* instance = NULL;
		if (instance == NULL) instance = new GameStateMachine();
		return instance;
	}
}

GameStateMachine::GameStateMachine()
{
	stackState.push(GameState::MAINMENU);
	activeState = GameState::MAINMENU;
}

GameStateMachine::~GameStateMachine()
{
}
