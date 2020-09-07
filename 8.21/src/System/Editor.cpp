#include "Editor.h"

#include "../src/Utility/Clock.h"
#include "../src/Resources/Window.h"
#include "../src/System/ResourceManager.h"
#include "../src/System/InputManager.h"
#include "../src/System/GUIManager.h"

#include <cassert>

Editor::Editor() :
	_exit		( false )
{
	if (!glfwInit()) {
		//...
		assert(NULL);
	}

	Clock* clock = new Clock;
	_environment.set_clock(clock);

	Window* window = new Window;
	_environment.set_window(window);

	ResourceManager* resource_manager = new ResourceManager;
	_environment.set_resource_manager(resource_manager);
	resource_manager->load_resources();

	InputManager* input_manager = new EditorInputManager;
	_environment.set_input_manager(input_manager);

	GUIManager* gui_manager = new GUIManager;
	_environment.set_gui_manager(gui_manager);
}

Editor::~Editor() {
	_environment.shut_down();
	glfwTerminate();
}

#include "../src/Resources/GUI.h"
#include "../src/Resources/GUIText.h"
void Editor::run() {

	std::shared_ptr<GUISelectionGrid> ui_test = std::make_shared<GUISelectionGrid>(.2f, .2f, glm::vec2(.0f, .0f), glm::vec4(1, 0, 1, .5f));
	std::shared_ptr<GUISelectionGrid> ui_test2 = std::make_shared<GUISelectionGrid>(.2f, .2f, glm::vec2(.0f, .0f), glm::vec4(0, 1, 1, .5f));
	std::shared_ptr<GUISelectionGrid> ui_test3 = std::make_shared<GUISelectionGrid>(.2f, .2f, glm::vec2(.0f, .0f), glm::vec4(1, 1, 0, .5f));
	std::shared_ptr<GUIMaster> ui_master = std::make_shared<GUIMaster>(.2f, .4f, glm::vec2(.1f, .1f), glm::vec4(1, 0, 0, .5f));

	auto icon = Environment::get().get_resource_manager()->get_icon(0);
	ui_test->add(icon);

	ui_master->add(ui_test);
	ui_master->add(ui_test2);
	ui_master->add(ui_test3);
	_environment.get_gui_manager()->add((ui_master));

	while (!_exit) {
		_environment.get_clock()->update();
		_environment.get_window()->update();

		render();

		_environment.get_input_manager()->update(&_exit);
		_environment.get_gui_manager()->update();
	}

}

void Editor::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, WINDOW_BACKGROUND_COLOR);

	_environment.get_resource_manager()->draw();

	_environment.get_gui_manager()->draw();

	glfwSwapBuffers(_environment.get_window()->get_glfw_window());
}