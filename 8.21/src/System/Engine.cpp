#include "Engine.h"

#include "../src/Utility/Clock.h"
#include "../src/Resources/Window.h"
#include "../src/System/ResourceManager.h"
#include "../src/System/InputManager.h"
#include "../src/System/GUIManager.h"
#include "../src/System/Renderer.h"

#include <cassert>

Engine::Engine() :
	_exit			 ( false )
{
	if(!glfwInit()) {
		//...
		assert(NULL);
	}

	_environment.set_mode(MODE_ENGINE);

	Clock* clock = new Clock;
	_environment.set_clock(clock);

	Window* window = new Window;
	_environment.set_window(window);

	ResourceManager* resource_manager = new ResourceManager;
	_environment.set_resource_manager(resource_manager);
	resource_manager->load_resources();

	InputManager* input_manager = new GameInputManager;
	_environment.set_input_manager(input_manager);

	GUIManager* gui_manager = new GUIManager;
	_environment.set_gui_manager(gui_manager);

	Renderer* renderer = new Renderer;
	_environment.set_renderer(renderer);
}

Engine::~Engine() {
	_environment.shut_down();
	glfwTerminate();
}

void Engine::run() {

	_environment.get().get_resource_manager()->new_entity("Unit", 0);

	while (!_exit) {
		_environment.get_clock()->update();
		_environment.get_window()->update();

		render();

		_environment.get_input_manager()->update(&_exit);
		_environment.get_resource_manager()->update();
	}
	
}

void Engine::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, WINDOW_BACKGROUND_COLOR);

	_environment.get_resource_manager()->draw();

	_environment.get_renderer()->debug_draw();

	glfwSwapBuffers(_environment.get_window()->get_glfw_window());
}