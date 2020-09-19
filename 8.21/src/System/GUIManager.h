#ifndef GUI_MANAGER_H
#define GUI_MANAGER_H

#include "../src/Resources/GUI.h"
#include "../src/Resources/GUIText.h"

#include <memory>

/********************************************************************************************************************************************************/

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

	void draw_text(GUITextDesc text_desc, GUIMasterDesc master_desc = GUIMasterDesc());
	void draw_icon(GUIIconDesc icon_desc, GUIMasterDesc master_desc = GUIMasterDesc());
	void draw_element(GUIDrawDesc draw_desc, GUIMasterDesc master_desc = GUIMasterDesc());
protected:
	std::vector<std::shared_ptr<GUIMaster>> _masters;
	std::vector<std::shared_ptr<GUIElement>> _elements;

	GUIDrawText _draw_text;
	GUIDrawIcon _draw_icon;
	GUIDrawElement _draw_element;
};

/********************************************************************************************************************************************************/

class GUIEntitySelection {
public:
	GUIEntitySelection();

	std::shared_ptr<GUIIcon> get_selection();
protected:
	std::shared_ptr<GUIMaster> _master;
	std::shared_ptr<GUISelectionGrid> _object_grid;
	//...

	std::shared_ptr<GUISelectionView> _selection_view;
};

/********************************************************************************************************************************************************/

class GUIEditorButtons {
public:
	GUIEditorButtons();

protected:
	std::shared_ptr<GUIButtonIcon> _bt_select_entity;
	std::shared_ptr<GUIButtonIcon> _bt_place_entity;
	std::shared_ptr<GUIButtonIcon> _bt_edit_terrain;
};

/********************************************************************************************************************************************************/

class EditorGUIManager : public GUIManager, public GUIEntitySelection, public GUIEditorButtons {
public:
	EditorGUIManager();
	~EditorGUIManager();

private:
	
};

/********************************************************************************************************************************************************/

#endif