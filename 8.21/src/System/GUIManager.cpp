#include "GUIManager.h"

#include <GL/gl3w.h>

#include "Environment.h"
#include "../src/System/ResourceManager.h"
#include "../src/Entities/Entity.h"

GUIManager::GUIManager() {
}

GUIManager::~GUIManager() {
}

void GUIManager::update() {
	for(const auto master : _masters) {
		master->update();
	}
}

void GUIManager::add(std::shared_ptr<GUIMaster> master) {
	_masters.push_back(master);
}

void GUIManager::draw() {
	for(const auto master : _masters) {
		master->draw(GL_TRIANGLES);
	}	
}

void GUIManager::click() {
	for(const auto master : _masters) {
		if(master->selected()) {
			master->click();
		}
	}
}

bool GUIManager::selected() {
	for(const auto master : _masters) {
		if(master->selected()) {
			return true;
		}
	}
	return false;
}

void GUIManager::scroll(double yoffset) {
	for(const auto master : _masters) {
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

GUIEntityGrid::GUIEntityGrid() :
	_master			( std::make_shared<GUIMaster>(.2f, .4f, glm::vec2(.1f, .1f), glm::vec4(1, 0, 0, .5f)) ),
	_object_grid	( std::make_shared<GUISelectionGrid>(.2f, .2f, glm::vec2(.0f, .0f), glm::vec4(1, 0, 1, .5f)) )
{
	_master->add(_object_grid);

	const auto icons = Environment::get().get_resource_manager()->get_icons();
	for(auto it = icons->begin(); it != icons->end(); ++it) {
		if(it->second->_type == ENTITY_OBJECT) {
			_object_grid->add(it->second);
		}
	}
}

std::shared_ptr<GUIIcon> GUIEntityGrid::get_selection() {
	const auto object_selection = _object_grid->get_selection();
	if(object_selection) {
		return object_selection;
	}

	return nullptr;
}

EditorGUIManager::EditorGUIManager() {
	_masters.push_back(GUIEntityGrid::_master);
}

EditorGUIManager::~EditorGUIManager() {}