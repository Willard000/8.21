#include "InputManager.h"

#include "../src/System/Environment.h"
#include "../src/Resources/Window.h"
#include "../src/Resources/Camera.h"
#include "../src/System/ResourceManager.h"
#include "../src/System/GUIManager.h"

#include "../src/Resources/Terrain.h"
#include "../src/Entities/Entity.h"

#include <GLFW/glfw3.h>

#include <cmath>

/********************************************************************************************************************************************************/

constexpr float SCROLL_SPEED = 50000.0f;

#include <iostream>
void select_entity() {
	const auto terrain = Environment::get().get_resource_manager()->get_terrain();
	const auto world_space = Environment::get().get_input_manager()->get_mouse_world_space_vector();
	const auto camera = Environment::get().get_window()->get_camera();

	const auto entity = terrain->select_entity(world_space, camera->get_position());

	if (entity) {
		std::cout << entity->get_name() << '\n';
	}
}

void select_tile() {
	const auto terrain = Environment::get().get_resource_manager()->get_terrain();
	const auto world_space = Environment::get().get_input_manager()->get_mouse_world_space_vector();
	const auto camera = Environment::get().get_window()->get_camera();

	terrain->select(world_space, camera->get_position());
}

void place_entity() {
	const auto gui_manager = (EditorGUIManager*)Environment::get().get_gui_manager();
	const auto selection = gui_manager->get_selection();
	const auto resource_manager = Environment::get().get_resource_manager();

	if (!selection) {
		return;
	}

	const auto terrain = Environment::get().get_resource_manager()->get_terrain();
	const auto world_space = Environment::get().get_input_manager()->get_mouse_world_space_vector();
	const auto camera = Environment::get().get_window()->get_camera();

	if (terrain->is_valid_tile() && terrain->is_empty_tile()) {
		const auto entity = resource_manager->new_entity(selection->_type, selection->_id);
		terrain->add_entity(entity);
	}
}

/********************************************************************************************************************************************************/

InputManager::InputManager() :
	_xpos			( 0.0 ),
	_ypos			( 0.0 ),
	_mode			( 0 )
{}

InputManager::~InputManager()
{}

double InputManager::get_mouse_x(){
	return _xpos;
}

double InputManager::get_mouse_y() {
	return _ypos;
}

glm::vec3 InputManager::get_mouse_world_space_vector() {
	const auto window = Environment::get().get_window()->get_glfw_window();
	const auto camera = Environment::get().get_window()->get_camera();
	const auto width = Environment::get().get_window()->get_width();
	const auto height = Environment::get().get_window()->get_height();

	const float xpos_norm = (float)_xpos / float(width / 2) - 1.0f;
	const float ypos_norm = -((float)_ypos / float(height / 2) - 1.0f);

	const glm::vec4 clip_space(xpos_norm, ypos_norm, -1.0f, 1.0f);
	const glm::mat4 inverse_projection = glm::inverse(camera->get_projection());

	glm::vec4 view_space = inverse_projection * clip_space;
	view_space.z = -1.0f;
	view_space.w = 0.0f;

	const glm::mat4 inverse_view = glm::inverse(camera->get_view());

	glm::vec3 world_space = inverse_view * view_space;
	world_space = glm::normalize(world_space);

	return world_space;
}

void InputManager::set_mode(int mode) {
	_mode = mode;
}

int InputManager::get_mode() {
	return _mode;
}

/********************************************************************************************************************************************************/

GameInputManager::GameInputManager()
{
	const auto window = Environment::get().get_window()->get_glfw_window();
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
}

GameInputManager::~GameInputManager()
{}

void GameInputManager::update(bool* exit)
{
	const auto window = Environment::get().get_window()->get_glfw_window();
	const auto camera = Environment::get().get_window()->get_camera();

	glfwPollEvents();

	if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
		return;
	}

	if (glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE)) {
		*exit = true;
	}

	glfwGetCursorPos(window, &_xpos, &_ypos);

	if (camera->get_mode() == CAMERA_FREE) {
		const auto width = Environment::get().get_window()->get_width();
		const auto height = Environment::get().get_window()->get_height();
		camera->move_angle((float)_xpos, (float)_ypos);
		glfwSetCursorPos(window, width / 2, height / 2);
	}

	if (glfwGetKey(window, GLFW_KEY_W)) {
		camera->move(CAMERA_FORWARD);
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
		camera->move(CAMERA_BACKWARD);
	}
	if (glfwGetKey(window, GLFW_KEY_A)) {
		camera->move(CAMERA_LEFT);
	}
	if (glfwGetKey(window, GLFW_KEY_D)) {
		camera->move(CAMERA_RIGHT);
	}
	if (glfwGetKey(window, GLFW_KEY_Q)) {
		camera->move(CAMERA_DOWN);
	}
	if (glfwGetKey(window, GLFW_KEY_E)) {
		camera->move(CAMERA_UP);
	}
}

void GameInputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
		return;
	}

	const auto camera = Environment::get().get_window()->get_camera();

	if(key == GLFW_KEY_1 && action == GLFW_PRESS) {
		camera->mode(CAMERA_TOGGLE);
	}
}

void GameInputManager::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
		return;
	}

	const auto camera = Environment::get().get_window()->get_camera();

	if (yoffset < 0) {
		camera->move(CAMERA_UP, SCROLL_SPEED);
	}
	if (yoffset > 0) {
		camera->move(CAMERA_DOWN, SCROLL_SPEED);
	}
}

void GameInputManager::cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
	if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
		return;
	}

	const auto camera = Environment::get().get_window()->get_camera();

	if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
		return;
	}

	if(camera->get_mode() == CAMERA_FREE) {
		camera->move_angle((float)xpos, (float)ypos);
	}
}

void GameInputManager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
		return;
	}
}

/********************************************************************************************************************************************************/

EditorInputManager::EditorInputManager() {
	_mode = EDITOR_EDIT_TERRAIN;

	auto window = Environment::get().get_window()->get_glfw_window();
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
}

EditorInputManager::~EditorInputManager()
{}

void EditorInputManager::update(bool* exit) {
	const auto window = Environment::get().get_window()->get_glfw_window();
	const auto camera = Environment::get().get_window()->get_camera();

	glfwPollEvents();

	if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
		return;
	}

	if (glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE)) {
		*exit = true;
	}

	glfwGetCursorPos(window, &_xpos, &_ypos);

	if (camera->get_mode() == CAMERA_FREE) {
		const auto width = Environment::get().get_window()->get_width();
		const auto height = Environment::get().get_window()->get_height();
		camera->move_angle((float)_xpos, (float)_ypos);
		glfwSetCursorPos(window, width / 2, height / 2);
	}

	if (glfwGetKey(window, GLFW_KEY_W)) {
		camera->move(CAMERA_FORWARD);
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
		camera->move(CAMERA_BACKWARD);
	}
	if (glfwGetKey(window, GLFW_KEY_A)) {
		camera->move(CAMERA_LEFT);
	}
	if (glfwGetKey(window, GLFW_KEY_D)) {
		camera->move(CAMERA_RIGHT);
	}
	if (glfwGetKey(window, GLFW_KEY_Q)) {
		camera->move(CAMERA_DOWN);
	}
	if (glfwGetKey(window, GLFW_KEY_E)) {
		camera->move(CAMERA_UP);
	}

	select_tile();
}

void EditorInputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
		return;
	}

	const auto camera = Environment::get().get_window()->get_camera();

	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		camera->mode(CAMERA_TOGGLE);
	}
}

void EditorInputManager::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
		return;
	}

	const auto camera = Environment::get().get_window()->get_camera();
	const bool GUI_selected = Environment::get().get_gui_manager()->selected();

	if (GUI_selected) {
		Environment::get().get_gui_manager()->scroll(yoffset);
	}
	else {
		if (yoffset < 0) {
			camera->move(CAMERA_UP, SCROLL_SPEED);
		}
		if (yoffset > 0) {
			camera->move(CAMERA_DOWN, SCROLL_SPEED);
		}
	}
}

void EditorInputManager::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
		return;
	}

	const auto camera = Environment::get().get_window()->get_camera();


	if (camera->get_mode() == CAMERA_FREE) {
		camera->move_angle((float)xpos, (float)ypos);
	}
}

void EditorInputManager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
		return;
	}

	const bool GUI_selected = Environment::get().get_gui_manager()->selected();
	const int mode = Environment::get().get_input_manager()->get_mode();

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if(GUI_selected) {
			Environment::get().get_gui_manager()->click();
		}
		else if (mode == EDITOR_EDIT_TERRAIN) {
			Environment::get().get_resource_manager()->get_terrain()->adjust_tile_height(1);
		}
		else if (mode == EDITOR_SELECT_ENTITY) {
			select_entity();
		}
		else if (mode == EDITOR_PLACE_ENTITY) {
			place_entity();
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (GUI_selected) {

		}
		else if (mode == EDITOR_EDIT_TERRAIN) {
			Environment::get().get_resource_manager()->get_terrain()->adjust_ramp_height();
		}
	}
}

/********************************************************************************************************************************************************/