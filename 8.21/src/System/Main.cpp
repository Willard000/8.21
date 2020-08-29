#include <conio.h>
#include <iostream>

#include "Engine.h"
#include "Editor.h"

int main() {

	int input = _getch();
	if(input == 49) {
		std::cout << "Editor" << '\n';
		Editor editor;
		editor.run();
	}
	else {
		std::cout << "Engine" << '\n';
		Engine engine;
		engine.run();
	}

	return 0;
}