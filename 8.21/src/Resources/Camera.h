#ifndef CAMERA_H
#define CAMERA_H

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#define CAMERA_FORWARD 1
#define CAMERA_BACKWARD 2
#define CAMERA_LEFT 3
#define CAMERA_RIGHT 4
#define CAMERA_UP 5
#define CAMERA_DOWN 6

#define CAMERA_FREE 0
#define CAMERA_LOCKED 1
#define CAMERA_TOGGLE 2

class Camera {
public:
	Camera(
		int* window_w,
		int* window_h,
		float fov,
		float aspect,
		float z_near,
		float z_far,
		float horizontal_angle,
		float vertical_angle,
		glm::vec3 position
	);

	void move(const int direction, float speed = 0.0f);
	void move_free(const int direction, float speed = 0.0f);
	void move_locked(const int direction, float speed = 0.0f);
	void move_angle(const float xpos, const float ypos);

	void mode(int mode);
	void update();

	void attach_shader(const GLuint program);
	void detach_shader(const GLuint program);

	int get_mode();

	glm::vec3 get_direction();
	glm::vec3 get_position();

	glm::mat4 get_projection();
	glm::mat4 get_view();

	float get_horizontal_angle();
	float get_vertical_angle();

	void set_position(glm::vec3 position);
private:
	int* _window_w;
	int* _window_h;

	int _mode;

	float _fov;
	float _aspect;

	float _z_near;
	float _z_far;

	float _horizontal_angle;
	float _vertical_angle;

	glm::vec3 _direction;
	glm::vec3 _right;
	glm::vec3 _up;

	glm::mat4 _projection;
	glm::mat4 _gui_projection;
	glm::mat4 _view;

	glm::vec3 _position;

	std::vector<GLuint> _programs;
};

#endif