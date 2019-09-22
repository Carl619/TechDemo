#include "menu.h"
#include "../../Common/Window.h"
using namespace NCL;
Menu::Menu(std::string Name)
{
	this->Name = Name;
	activated = false;
	CurrentOption = 0;
}

Menu::~Menu()
{
	Options.clear();
}

void Menu::controller()
{
	if (Window::GetKeyboard()->KeyPressed(KEYBOARD_W)) {
		if (CurrentOption == 0) {
			CurrentOption = Options.size() - 1;
		}
		else{
			CurrentOption--;
		}
	}
	else if (Window::GetKeyboard()->KeyPressed(KEYBOARD_S)) {
		if (CurrentOption == Options.size() - 1) {
			CurrentOption = 0;
		}
		else {
			CurrentOption++;
		}
	}
	else if (Window::GetKeyboard()->KeyPressed(KEYBOARD_E)) {
		executeOption();
	}
}
