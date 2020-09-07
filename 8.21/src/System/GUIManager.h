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

	void draw_text(GUITextDesc text_desc, GUIMasterDesc master_desc);
	void draw_icon(GUIIconDesc icon_desc);
private:
	std::vector<std::shared_ptr<GUIMaster>> _masters;

	GUIDrawText _draw_text;
	GUIDrawIcon _draw_icon;
};

#endif