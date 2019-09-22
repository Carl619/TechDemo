#pragma once
#include "menu.h"

class MainMenu : public Menu {
public:
	MainMenu(std::string Name) :Menu(Name) {
		Options.push_back("Game");
		Options.push_back("Online");
		Options.push_back("Exit");
	};
	~MainMenu();
	void executePlayGameOption();
	void executeExitOption();
	void executeOnlineOption();
	void executeOption();
};