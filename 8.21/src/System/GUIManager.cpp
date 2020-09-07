#include "GUIManager.h"

#include <GL/gl3w.h>

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

void GUIManager::draw_icon(GUIIconDesc icon_desc) {
	_draw_icon.draw(icon_desc);
}