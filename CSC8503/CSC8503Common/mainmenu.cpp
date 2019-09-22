#include "mainmenu.h"
#include "gamestatemachine.h"
MainMenu::~MainMenu()
{
	Options.clear();
}

void MainMenu::executePlayGameOption()
{
	NCL::CSC8503::GameStateMachine::getInstance()->pushState(NCL::CSC8503::GameState::LEVELMENU);
}

void MainMenu::executeExitOption()
{
	NCL::CSC8503::GameStateMachine::getInstance()->replace(NCL::CSC8503::GameState::EXIT);
}

void MainMenu::executeOnlineOption()
{
	NCL::CSC8503::GameStateMachine::getInstance()->pushState(NCL::CSC8503::GameState::ONLINE);
}

void MainMenu::executeOption()
{
	if (CurrentOption == 0)
		executePlayGameOption();
	else if (CurrentOption == 1)
		executeOnlineOption();
	else if (CurrentOption == 2)
		executeExitOption();
}
