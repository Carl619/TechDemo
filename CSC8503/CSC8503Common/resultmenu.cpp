#include "resultmenu.h"
#include "gamestatemachine.h"
ResultMenu::~ResultMenu()
{
	Options.clear();
}

void ResultMenu::executePlayGameOption()
{
	if(NCL::CSC8503::GameStateMachine::getInstance()->getActiveState() == NCL::CSC8503::GameState::ONLINERESULTCLIENT)
		NCL::CSC8503::GameStateMachine::getInstance()->changeState(NCL::CSC8503::GameState::ONLINE);
	else
		NCL::CSC8503::GameStateMachine::getInstance()->replace(NCL::CSC8503::GameState::LOADING);
}

void ResultMenu::executeExitOption()
{
	if (NCL::CSC8503::GameStateMachine::getInstance()->getActiveState() == NCL::CSC8503::GameState::ONLINERESULTCLIENT)
		NCL::CSC8503::GameStateMachine::getInstance()->replace(NCL::CSC8503::GameState::DISCONNECTCLIENT);
	else
		NCL::CSC8503::GameStateMachine::getInstance()->popState();
}

void ResultMenu::executeOption()
{
	if (CurrentOption == 0)
		executePlayGameOption();
	else if (CurrentOption == 1)
		executeExitOption();
}
