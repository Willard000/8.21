#ifndef EDITOR_H
#define EDITOR_H

#include "Environment.h"

class Editor {
public:
	Editor();
	~Editor();

	void run();
	void render();
private:
	bool _exit;

	Environment _environment;
};

#endif