#include "Window.h"

#include "../src/Utility/FileReader.h"

#include "../src/System/Environment.h"
#include "../src/Utility/Clock.h"
#include "../src/System/GUIManager.h"

#include "Camera.h"

#include "../src/Utility/Timer.h"

#include <cassert>

ReadWindowFile::ReadWindowFile(const char* file_path) {
	FileReader file(file_path);
	if (file.set_section("Window")) {
		file.read(&_width, "i_width");
		file.read(&_height, "i_height");
		file.read(&_title, "str_title");
		file.read(&_xpos, "i_xpos");
		file.read(&_ypos, "i_ypos");
	}
}

Window::Window(const ReadWindowFile window_file) :
	_window			( glfwCreateWindow(window_file._width, window_file._height, window_file._title.c_str(), nullptr, nullptr) ),
	_width			( window_file._width ),
	_height			( window_file._height )
{
	assert(_window);
	glfwMakeContextCurrent(_window);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwSetWindowPos(_window, window_file._xpos, window_file._ypos);

	if(gl3wInit()) {
		// ...
		assert(NULL);
	}

	//???
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	_camera = std::make_shared<Camera>(&_width, &_height, 90.0f, 1.0f, 0.01f, 1000.0f, 3.14f, 0.0f, glm::vec3(0.0f, 1.0f, 5.0f));

	glfwSetWindowSizeCallback(_window, window_size_callback);
}

Window::~Window() {
	glfwDestroyWindow(_window);
}

void Window::update() {
	static Timer update_title_timer(1.0);

	if (update_title_timer.update()) {
		glfwSetWindowTitle(_window, std::to_string(Environment::get().get_clock()->get_fms()).c_str());
	}

	_camera->update();
}

int Window::get_width() {
	return _width;
}

int Window::get_height() {
	return _height;
}

GLFWwindow* Window::get_glfw_window() {
	return _window;
}

std::shared_ptr<Camera> Window::get_camera() {
	return _camera;
}

void Window::window_size_callback(GLFWwindow* window, int width, int height) {
	Environment::get().get_window()->set_size(width, height);
	glViewport(0, 0, width, height);
}

void Window::set_size(int width, int height) {
	_width = width;
	_height = height;
}