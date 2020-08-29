#ifndef GUI_MANAGER_H
#define GUI_MANAGER_H

#include "../src/Resources/GUI.h"

#include <memory>

class GUIManager {
public:
	GUIManager();
	~GUIManager();

	void add(std::shared_ptr<GUIElement> element);
	void draw();
private:
	std::vector<std::shared_ptr<GUIElement>> _elements;
};

#endif