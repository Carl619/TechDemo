#pragma once
#include <vector>
#include <string>

class Menu {
public:
	Menu(std::string Name);
	~Menu();
	void activate() { activated = !activated; }
	bool isActivated() { return activated;}
	int getCUrrentOption() { return CurrentOption; }
	virtual void executeOption() = 0;
	void controller();
protected:
	std::vector<std::string> Options;
	std::string Name;
	bool activated;
	int CurrentOption;

};