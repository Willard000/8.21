#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <glm/gtc/matrix_transform.hpp>

struct GLFWwindow;

class InputManager {
public:
	InputManager();
	~InputManager();

	virtual void update(bool* exit) = 0;

	double get_mouse_x();
	double get_mouse_y();

	glm::vec3 get_mouse_world_space_vector();
protected:
	double _xpos;
	double _ypos;
};

class GameInputManager : public InputManager {
public:
	GameInputManager();
	~GameInputManager();

	void update(bool* exit);

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
private:
};

class EditorInputManager : public InputManager {
public:
	EditorInputManager();
	~EditorInputManager();

	void update(bool* exit);

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
private:
};

#endif