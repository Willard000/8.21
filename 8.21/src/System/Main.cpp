#include <conio.h>
#include <iostream>

#include "Engine.h"
#include "Editor.h"
#include "../Network/Server.h"

#include <thread>

void start_engine() {
	std::cout << "Engine" << '\n';
	Engine engine;
	engine.run();
}

void start_editor() {
	std::cout << "Editor" << '\n';
	Editor editor;
	editor.run();
}

void start_server() {
	std::cout << "Server" << '\n';
	Server server;
	server.s_startup();


	auto thread = std::thread(&Server::s_accept, &server);

	while(1) {

	}
}

int main() {

	int input = _getch();

	if(input == 49) {
		start_editor();
	}
	else if(input == 50) {
		start_server();
	}
	else {
		start_engine();
	}

	return 0;
}