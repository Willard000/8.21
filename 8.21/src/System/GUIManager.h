#ifndef GUI_MANAGER_H
#define GUI_MANAGER_H

#include "../src/Resources/GUI.h"

#include <memory>

class GUIManager {
public:
	GUIManager();
	~GUIManager();

	void update();
	void add(std::shared_ptr<GUIMaster> master);
	void draw();
	bool selected();
private:
	std::vector<std::shared_ptr<GUIMaster>> _masters;
};

#endif