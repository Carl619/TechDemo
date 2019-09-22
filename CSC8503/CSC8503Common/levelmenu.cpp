#include "levelmenu.h"
#include "gamestatemachine.h"

LevelMenu::~LevelMenu()
{
	Options.clear();
}

void LevelMenu::executeLevel1Option()
{
	NCL::CSC8503::GameStateMachine::getInstance()->replace(NCL::CSC8503::GameState::LEVEL1);
}

void LevelMenu::executeLevel2Option()
{
	NCL::CSC8503::GameStateMachine::getInstance()->replace(NCL::CSC8503::GameState::LEVEL2);
}

void LevelMenu::executeLevel3Option()
{
	NCL::CSC8503::GameStateMachine::getInstance()->replace(NCL::CSC8503::GameState::LEVEL3);
}


void LevelMenu::executeBackOption()
{
	NCL::CSC8503::GameStateMachine::getInstance()->popState();
}

void LevelMenu::executeOption()
{
	if (CurrentOption == 0)
		executeLevel1Option();
	else if (CurrentOption == 1)
		executeLevel2Option();
	else if (CurrentOption == 2)
		executeLevel3Option();
	else if (CurrentOption == 3)
		executeBackOption();
}
