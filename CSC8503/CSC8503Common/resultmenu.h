#pragma once
#include "menu.h"

class ResultMenu : public Menu {
public:
	ResultMenu(std::string Name) :Menu(Name) {
		Options.push_back("Replay");
		Options.push_back("Back");
	};
	~ResultMenu();
	void executePlayGameOption();
	void executeExitOption();
	void executeOption();
};