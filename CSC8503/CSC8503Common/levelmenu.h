#pragma once
#include "menu.h"

class LevelMenu : public Menu {
public:
	LevelMenu(std::string Name) :Menu(Name) {
		Options.push_back("Level 1");
		Options.push_back("Level 2");
		Options.push_back("Level 3");
		Options.push_back("Back");
	};
	~LevelMenu();
	void executeLevel1Option();
	void executeLevel2Option();
	void executeLevel3Option();
	void executeBackOption();
	void executeOption();
};