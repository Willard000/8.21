#ifndef GUI_MANAGER_H
#define GUI_MANAGER_H

#include "../src/Resources/GUI.h"
#include "../src/Resources/GUIText.h"

#include <memory>

class GUIManager {
public:
	GUIManager();
	~GUIManager();

	void update();
	void add(std::shared_ptr<GUIMaster> master);
	void draw();
	bool selected();
	void scroll(double yoffset);
private:
	std::vector<std::shared_ptr<GUIMaster>> _masters;
};

#endif