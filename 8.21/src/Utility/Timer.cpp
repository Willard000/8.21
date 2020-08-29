#include "Timer.h"

#include <GLFW/glfw3.h>

Timer::Timer() :
	_time				( 0.0 ),
	_clock				( 0.0 ),
	_prev_time			( glfwGetTime() ),
	_is_paused			( true )
{}

Timer::Timer(const double time) :
	_time				( time ),
	_clock				( 0 ),
	_prev_time			( glfwGetTime() ),
	_is_paused			( false )

{}

Timer::Timer(const Timer& rhs) :
	_time				( rhs._time ),
	_clock				( rhs._clock ),
	_prev_time			( rhs._prev_time ),
	_is_paused			( rhs._is_paused )
{}

void Timer::set(const double time) {
	_time = time;
	_is_paused = false;
}

void Timer::pause() {
	if (_is_paused) {
		_prev_time = glfwGetTime() - _clock;
	}
	else {
		_clock = glfwGetTime() - _prev_time;
	}
	_is_paused = !_is_paused;
}

void Timer::reset() {
	_prev_time = glfwGetTime();
}

void Timer::offset(const double time) {
	_prev_time -= time;
}

bool Timer::update() {
	if (_time > -1 && !_is_paused && ((glfwGetTime() - _prev_time) > _time)) {
		_prev_time = glfwGetTime();
		return true;
	}
	return false;
}

bool Timer::get_pause() {
	return _is_paused;
}

double Timer::get_time() {
	return _time;
}

double Timer::get_clock() {
	if (!_is_paused) {
		_clock = glfwGetTime() - _prev_time;
	}
	return _clock;
}