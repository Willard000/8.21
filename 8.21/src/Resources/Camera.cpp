#include "Camera.h"

#include "../src/System/Environment.h"
#include "../src/Utility/Clock.h"

constexpr float SPEED = 10000.0f;
constexpr float HALF_PIE = 3.14159f / 2.0f;
constexpr float LOCKED_VERTICAL_ANGLE = -1.0f;

Camera::Camera(
	int*   window_w,
	int*   window_h,
	float fov,
	float aspect,
	float z_near,
	float z_far,
	float horizontal_angle,
	float vertical_angle,
	glm::vec3 position
) :
	_window_w					( window_w ),
	_window_h					( window_h ),
	_mode						( CAMERA_LOCKED ),
	_fov						( fov ),
	_aspect						( aspect ),
	_z_near						( z_near ),
	_z_far						( z_far ),
	_horizontal_angle			( horizontal_angle ),
	_vertical_angle				( LOCKED_VERTICAL_ANGLE ),
	_position					( position ),
	_direction					( glm::vec3(0, 0, 0) ),
	_right						( glm::vec3(0, 0, 0) ),
	_up							( glm::vec3(0, 0, 0) ),
	_view						( glm::mat4() ),
	_projection					( glm::perspective(fov, aspect, z_near, z_far) )
{}

void Camera::attach_shader(GLuint program) {
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, &_projection[0][0]);

	_programs.push_back(program);
}

void Camera::detach_shader(GLuint program) {
	for (auto it = _programs.begin(); it != _programs.end(); ++it) {
		if (*it == program) {
			_programs.erase(it);
		}
	}
}

void Camera::mode(int mode) {
	if (mode == CAMERA_FREE) {
		_mode = mode;
	}
	else if (mode == CAMERA_LOCKED) {
		_vertical_angle = LOCKED_VERTICAL_ANGLE;
		_horizontal_angle = 3.14f;
		_mode = mode;
	}
	else if (mode == CAMERA_TOGGLE) {
		if (_mode == CAMERA_FREE) {
			_mode = CAMERA_LOCKED;
			_vertical_angle = LOCKED_VERTICAL_ANGLE;
			_horizontal_angle = 3.14f;
		}
		else if (_mode == CAMERA_LOCKED) {
			_mode = CAMERA_FREE;
		}
	}
}

void Camera::move_free(const int direction, float speed) {
	if (direction == CAMERA_FORWARD) {
		_position.x += _direction.x * (SPEED + speed) * (float)Environment::get().get_clock()->get_time();
		_position.z += _direction.z * (SPEED + speed) * (float)Environment::get().get_clock()->get_time();
	}

	if (direction == CAMERA_BACKWARD) {
		_position.x -= _direction.x * (SPEED + speed) * (float)Environment::get().get_clock()->get_time();
		_position.z -= _direction.z * (SPEED + speed) * (float)Environment::get().get_clock()->get_time();
	}

	if (direction == CAMERA_LEFT) {
		_position.x -= _right.x * (float)Environment::get().get_clock()->get_time() * (SPEED + speed);
		_position.z -= _right.z * (float)Environment::get().get_clock()->get_time() * (SPEED + speed);
	}

	if (direction == CAMERA_RIGHT) {
		_position.x += _right.x * (float)Environment::get().get_clock()->get_time() * (SPEED + speed);
		_position.z += _right.z * (float)Environment::get().get_clock()->get_time() * (SPEED + speed);
	}

	if (direction == CAMERA_UP) {
		_position.y += (float)Environment::get().get_clock()->get_time() * (SPEED + speed);
	}

	if (direction == CAMERA_DOWN) {
		_position.y -= (float)Environment::get().get_clock()->get_time() * (SPEED + speed);
	}
}

void Camera::move(const int direction, float speed) {
	switch (_mode) {
	case CAMERA_FREE:
		move_free(direction, speed);
		break;
	case CAMERA_LOCKED:
		move_locked(direction, speed);
		break;
	}
}

void Camera::move_locked(const int direction, float speed) {
	if (direction == CAMERA_FORWARD) {
		_position.z -= (SPEED + speed) * (float)Environment::get().get_clock()->get_time();
	}

	if (direction == CAMERA_BACKWARD) {
		_position.z += (SPEED + speed) * (float)Environment::get().get_clock()->get_time();
	}

	if (direction == CAMERA_LEFT) {
		_position.x -= (float)Environment::get().get_clock()->get_time() * (SPEED + speed);
	}

	if (direction == CAMERA_RIGHT) {
		_position.x += (float)Environment::get().get_clock()->get_time() * (SPEED + speed);
	}

	if (direction == CAMERA_UP) {
		_position.y += (float)Environment::get().get_clock()->get_time() * (SPEED + speed);
	}

	if (direction == CAMERA_DOWN) {
		_position.y -= (float)Environment::get().get_clock()->get_time() * (SPEED + speed);
	}
}

void Camera::move_angle(const float xpos, const float ypos) {
	_horizontal_angle += 0.001f * ((*_window_w / 2) - xpos);
	_vertical_angle += 0.001f * ((*_window_h / 2) - ypos);
}

void Camera::update() {
	_direction = glm::vec3(
		cos(_vertical_angle) * sin(_horizontal_angle),
		sin(_vertical_angle),
		cos(_vertical_angle) * cos(_horizontal_angle)
	);

	_right = glm::vec3(
		sin(_horizontal_angle - HALF_PIE),
		0,
		cos(_horizontal_angle - HALF_PIE)
	);

	_up = glm::cross(_right, _direction);

	_view = glm::lookAt(
		_position,
		_position + _direction,
		_up
	);

	for (auto p : _programs) {
		glUseProgram(p);
		glUniformMatrix4fv(glGetUniformLocation(p, "view"), 1, GL_FALSE, &_view[0][0]);
	}
}

int Camera::get_mode() {
	return _mode;
}

glm::vec3 Camera::get_direction() {
	return _direction;
}

glm::vec3 Camera::get_position() {
	return _position;
}

glm::mat4 Camera::get_projection() {
	return _projection;
}

glm::mat4 Camera::get_view() {
	return _view;
}

void Camera::set_position(glm::vec3 position) {
	_position = position;
}

float Camera::get_horizontal_angle() {
	return _horizontal_angle;
}

float Camera::get_vertical_angle() {
	return _vertical_angle;
}