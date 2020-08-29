#include "Environment.h"

#include "../src/Resources/Window.h"
#include "../src/Resources/Camera.h"
#include "../src/Utility/Clock.h"
#include "../src/System/ResourceManager.h"
#include "../src/System/InputManager.h"
#include "../src/System/GUIManager.h"

#include <cassert>

Environment* Environment::_instance = nullptr;

Environment::Environment() :
	_mode				( NULL    ),
	_clock				( nullptr ),
	_log				( nullptr ),
	_window				( nullptr ),
	_resource_manager	( nullptr ),
	_input_manager		( nullptr ),
	_gui_manager		( nullptr )
{
	assert(!_instance);
	_instance = this;
}

Environment::~Environment() {
	shut_down();
	_instance = nullptr;
}

Environment& Environment::get() {
	assert(_instance);
	return *_instance;
}

void Environment::set_mode(int mode) {
	_mode = mode;
}

void Environment::set_clock(Clock* clock) {
	_clock = clock;
}

void Environment::set_log(Log* log) {
	_log = log;
}

void Environment::set_window(Window* window) {
	_window = window;
}

void Environment::set_resource_manager(ResourceManager* resource_manager) {
	_resource_manager = resource_manager;
}

void Environment::set_input_manager(InputManager* input_manager) {
	_input_manager = input_manager;
}

void Environment::set_gui_manager(GUIManager* gui_manager) {
	_gui_manager = gui_manager;
}

Clock* Environment::get_clock() {
	return _clock;
}

Log* Environment::get_log() {
	return _log;
}

Window* Environment::get_window() {
	return _window;
}

ResourceManager* Environment::get_resource_manager() {
	return _resource_manager;
}

InputManager* Environment::get_input_manager() {
	return _input_manager;
}

GUIManager* Environment::get_gui_manager() {
	return _gui_manager;
}

void Environment::shut_down() {
	if (_window) {
		delete _window;
		_window = nullptr;
	}
	
	if (_log) {
		delete _log;
		_log = nullptr;
	}

	if (_clock) {
		delete _clock;
		_clock = nullptr;
	}

	if (_resource_manager) {
		delete _resource_manager;
		_resource_manager = nullptr;
	}

	if (_input_manager) {
		delete _input_manager;
		_input_manager = nullptr;
	}

	if (_gui_manager) {
		delete _gui_manager;
		_gui_manager = nullptr;
	}
}