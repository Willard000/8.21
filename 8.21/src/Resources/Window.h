#ifndef WINDOW_H
#define WINODW_H

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <string>
#include <memory>

#define WINDOW_FILE "Data/system.txt"

constexpr GLfloat WINDOW_BACKGROUND_COLOR[4] = { 0.0, 0.0, 0.0, 1.0 };

struct ReadWindowFile {
	ReadWindowFile(const char* file_path);

	int _width = 1000;
	int _height = 800;
	int _xpos = 0;
	int _ypos = 0;
	std::string _title = "Window";
};

class Camera;

class Window {
public:
	Window(const ReadWindowFile window_file = ReadWindowFile(WINDOW_FILE));
	~Window();

	void update();

	void hide();
	void show();

	int get_width();
	int get_height();
	GLFWwindow* get_glfw_window();
	std::shared_ptr<Camera> get_camera();

	static void window_size_callback(GLFWwindow* window, int width, int height);
private:
	void set_size(int width, int height);
private:
	GLFWwindow* _window;
	std::shared_ptr<Camera> _camera;

	int _width;
	int _height;
};

#endif