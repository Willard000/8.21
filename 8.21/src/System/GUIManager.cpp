#include "GUIManager.h"

#include <GL/gl3w.h>

GUIManager::GUIManager() {
}

GUIManager::~GUIManager() {
}

void GUIManager::add(std::shared_ptr<GUIElement> element) {
	_elements.push_back(element);
}

void GUIManager::draw() {
	for(const auto element : _elements) {
		if (element) {
			element->draw(GL_TRIANGLES);
		}
	}
}