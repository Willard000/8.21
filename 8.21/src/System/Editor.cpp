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
void Editor::run() {

	auto ui_test = std::make_shared<GUIElement>(100, 100, glm::vec2(.1, .1), glm::vec4(1, 0, 1, .5));
	_environment.get_gui_manager()->add(ui_test);

	while (!_exit) {
		_environment.get_clock()->update();
		_environment.get_window()->update();

		render();

		_environment.get_input_manager()->update(&_exit);
	}

}

void Editor::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, WINDOW_BACKGROUND_COLOR);

	_environment.get_resource_manager()->draw();

	_environment.get_gui_manager()->draw();

	glfwSwapBuffers(_environment.get_window()->get_glfw_window());
}