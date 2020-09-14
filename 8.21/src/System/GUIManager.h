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

	void click();

	bool selected();
	void scroll(double yoffset);

	void draw_text(GUITextDesc text_desc, GUIMasterDesc master_desc);
	void draw_icon(GUIIconDesc icon_desc, GUIMasterDesc master_desc);
protected:
	std::vector<std::shared_ptr<GUIMaster>> _masters;

	GUIDrawText _draw_text;
	GUIDrawIcon _draw_icon;
};

class GUIEntityGrid {
public:
	GUIEntityGrid();

	std::shared_ptr<GUIIcon> get_selection();
protected:
	std::shared_ptr<GUIMaster> _master;
	std::shared_ptr<GUISelectionGrid> _object_grid;
	//...
};

class EditorGUIManager : public GUIManager, public GUIEntityGrid {
public:
	EditorGUIManager();
	~EditorGUIManager();

private:
	
};

#endif