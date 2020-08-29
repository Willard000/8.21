#ifndef ENGINE_H
#define ENGINE_H

#include "Environment.h"

class Engine {
public:
	Engine();
	~Engine();

	void run();
	void render();
private:
	bool _exit;

	Environment _environment;
};

#endif