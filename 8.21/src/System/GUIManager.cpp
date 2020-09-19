#include "GUIManager.h"

#include <GL/gl3w.h>

#include "Environment.h"
#include "../src/System/ResourceManager.h"
#include "../src/Entities/Entity.h"

#include "../src/System/GUIFunctions.h"

/********************************************************************************************************************************************************/

GUIManager::GUIManager() {
}

GUIManager::~GUIManager() {
}

void GUIManager::update() {
	for(const auto& master : _masters) {
		master->update();
	}

	for(const auto& element : _elements) {
		element->select();
	}
}

void GUIManager::add(std::shared_ptr<GUIMaster> master) {
	_masters.push_back(master);
}

void GUIManager::draw() {
	for(const auto master : _masters) {
		master->draw(GL_TRIANGLES);
	}	

	for(const auto& element : _elements) {
		element->draw();
	}
}

void GUIManager::click() {
	for(const auto& master : _masters) {
		if(master->selected()) {
			master->click();
		}
	}

	for(const auto& element : _elements) {
		if(element->selected()) {
			element->click();
		}
	}
}

bool GUIManager::selected() {
	for(const auto& master : _masters) {
		if(master->selected()) {
			return true;
		}
	}

	for(const auto& element : _elements) {
		if(element->selected() ) {
			return true;
		}
	}

	return false;
}

void GUIManager::scroll(double yoffset) {
	for(const auto& master : _masters) {
		if(master->selected()) {
			master->scroll(yoffset);
		}
	}
}

void GUIManager::draw_text(GUITextDesc text_desc, GUIMasterDesc master_desc) {
	_draw_text.draw(text_desc, master_desc);
}

void GUIManager::draw_icon(GUIIconDesc icon_desc, GUIMasterDesc master_desc) {
	_draw_icon.draw(icon_desc, master_desc);
}

void GUIManager::draw_element(GUIDrawDesc draw_desc, GUIMasterDesc master_desc) {
	_draw_element.draw(draw_desc, master_desc);
}

/********************************************************************************************************************************************************/

GUIEntitySelection::GUIEntitySelection() :
	_master			( std::make_shared<GUIMaster>(.2f, .6f, glm::vec2(0.8f, 0.0f), glm::vec4(0, 0, 0, .3f)) ),
	_object_grid	( std::make_shared<GUISelectionGrid>(.2f, .6f, glm::vec2(.0f, .0f), glm::vec4(0, 0, 0, .3f)) ),
	_selection_view ( std::make_shared<GUISelectionView>(.2f, .4f, glm::vec2(.8f, .6f), glm::vec4(0, 0, 0, .3f)) )
{
	_master->add(_object_grid);

	_object_grid->set_view(_selection_view);
	_object_grid->set_title("Objects");
	const auto icons = Environment::get().get_resource_manager()->get_icons();
	for(auto it = icons->begin(); it != icons->end(); ++it) {
		if(it->second->_type == ENTITY_OBJECT) {
			_object_grid->add(it->second);
		}
	}
}

std::shared_ptr<GUIIcon> GUIEntitySelection::get_selection() {
	const auto object_selection = _object_grid->get_selection();
	if(object_selection) {
		return object_selection;
	}

	return nullptr;
}

/********************************************************************************************************************************************************/

GUIEditorButtons::GUIEditorButtons() :
	_bt_place_entity	( std::make_shared<GUIButtonIcon>(Environment::get().get_resource_manager()->get_icon(2), &bt_set_mode_place_entity, .03f, .05f, glm::vec2(.8f - .03f, 1.0f - .15f), glm::vec4(0, 0, 0, .3f)) ),
	_bt_select_entity	( std::make_shared<GUIButtonIcon>(Environment::get().get_resource_manager()->get_icon(1), &bt_set_mode_select_entity, .03f, .05f, glm::vec2(.8f - .03f, 1.0f - .1f), glm::vec4(0, 0, 0, .3f)) ),
	_bt_edit_terrain   ( std::make_shared<GUIButtonIcon>(Environment::get().get_resource_manager()->get_icon(0), &bt_set_mode_edit_terrain, .03f, .05f, glm::vec2(.8f - .03f, 1.0f - .05f), glm::vec4(0, 0, 0, .3f)) )
{}

/********************************************************************************************************************************************************/

EditorGUIManager::EditorGUIManager() {
	_masters.push_back(GUIEntitySelection::_master);
	_elements.push_back(GUIEntitySelection::_selection_view);

	_elements.push_back(GUIEditorButtons::_bt_place_entity);
	_elements.push_back(GUIEditorButtons::_bt_select_entity);
	_elements.push_back(GUIEditorButtons::_bt_edit_terrain);
}

EditorGUIManager::~EditorGUIManager() {}

/********************************************************************************************************************************************************/