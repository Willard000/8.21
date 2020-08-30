#include "GUI.h"

#include "../src/System/Environment.h"
#include "../src/System/InputManager.h"
#include "../src/System/ResourceManager.h"
#include "../src/Resources/Program.h"
#include "../src/Resources/Window.h"

#include <GL/gl3w.h>

#define GUI_SHADER 2

const glm::mat4 GUI_PROJECTION = glm::ortho(0, 1, 0, 1);

GUIPositionElement::GUIPositionElement() :
	_width			( 0.0f ),
	_height			( 0.0f ),
	_position		( glm::vec2(0, 0) ),
	_color			( glm::vec4(0, 0, 0, 0) )
{}

// position 0.0f - 1.0f ratio
GUIPositionElement::GUIPositionElement(float width, float height, glm::vec2 position, glm::vec4 color) :
	_width			( width ),
	_height			( height ),
	_position		( position ),
	_color			( color )
{}

GUIViewPort::GUIViewPort() :
	_position(glm::vec2(0, 0)),
	_width(0.0f),
	_height(0.0f)
{}

GUIViewPort::GUIViewPort(float width, float height) :
	_position(glm::vec2(0, 0)),
	_width(width),
	_height(height)
{}

glm::vec4 GUIViewPort::get_viewport() {
	const float window_width = (float)Environment::get().get_window()->get_width();
	const float window_height = (float)Environment::get().get_window()->get_height();
	return { (_position.x + GUIPositionElement::_position.x) * window_width,
			 (_position.y + GUIPositionElement::_position.y) * window_height,
			 _width * window_width,
			 _height * window_height
	};
}

GUIDrawElement::GUIDrawElement(float width, float height, glm::vec2 position, glm::vec4 color) :
	GUIPositionElement ( width, height, position, color )
{
	generate_vertex_data();
	create_vao();
}

GUIDrawElement::GUIDrawElement() {
	generate_vertex_data();
	create_vao();
}

GUIDrawElement::~GUIDrawElement() {
	glDeleteVertexArrays(1, &_vao);
}

void GUIDrawElement::draw(int mode, glm::vec4 viewport) {  
	glBindVertexArray(_vao);
	glUseProgram(_program);

	glUniform4fv(glGetUniformLocation(_program, "viewport"), 1, &viewport[0]);
	glUniform4fv(glGetUniformLocation(_program, "color"), 1, &_color[0]);
	glUniform2fv(glGetUniformLocation(_program, "position"), 1, &_position[0]);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glDrawArrays(mode, 0, _vertex_data.size());

	glDisable(GL_BLEND);
}

void GUIDrawElement::generate_vertex_data() {
	_vertex_data.reserve(6);
	_vertex_data.push_back(glm::vec2(0, 0));
	_vertex_data.push_back(glm::vec2(_width, 0));
	_vertex_data.push_back(glm::vec2(0, _height));
	_vertex_data.push_back(glm::vec2(0, _height));
	_vertex_data.push_back(glm::vec2(_width, 0));
	_vertex_data.push_back(glm::vec2(_width, _height));
}

void GUIDrawElement::create_vao() {
	glCreateVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	_program = Environment::get().get_resource_manager()->get_program(GUI_SHADER)->_id;
	glUseProgram(_program);
	glUniformMatrix4fv(glGetUniformLocation(_program, "projection"), 1, GL_FALSE, &GUI_PROJECTION[0][0]);

	glCreateBuffers(1, &_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
	glNamedBufferStorage(_vertex_buffer, sizeof(glm::vec2) * _vertex_data.size(), &_vertex_data[0], 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

GUISelectElement::GUISelectElement() :
	_mouse_x				( 0 ),
	_mouse_y				( 0 ),
	_valid					( false )
{}

GUISelectElement::GUISelectElement(float width, float height, glm::vec2 position, glm::vec4 color) :
	GUIPositionElement		( width, height, position, color),
	_mouse_x				( 0 ),
	_mouse_y				( 0 ),
	_valid					( false )
{}

bool GUISelectElement::selected() {
	return _valid;
}

void GUISelectElement::update() {
	const auto window = Environment::get().get_window();
	const float xpos = (float)Environment::get().get_input_manager()->get_mouse_x();
	const float ypos = (float)Environment::get().get_input_manager()->get_mouse_y();
	_mouse_x = int(xpos - (_position.x * (float)window->get_width()));
	_mouse_y = int(ypos - (float(window->get_height() - _height) - _position.y * (float)window->get_height()));
	_valid = _mouse_x <= _width && _mouse_x >= _position.x && _mouse_y <= _height && _mouse_y >= _position.y;
}

GUISelectionGrid::GUISelectionGrid(float width, float height, glm::vec2 position, glm::vec4 color) :
	GUIPositionElement		( width, height, position, color )
{}

void GUISelectionGrid::update() {
	GUISelectElement::update();
}

void GUISelectionGrid::draw(int mode, glm::vec4 viewport) {
	GUIDrawElement::draw(mode, viewport);
}

bool GUISelectionGrid::selected() {
	return GUISelectElement::selected();
}

GUIMaster::GUIMaster(float width, float height, glm::vec2 position, glm::vec4 color) :
	GUIPositionElement		( width, height, position, color ),
	GUIViewPort				( width, height )
{}

void GUIMaster::update() {
	for(const auto element : _elements) {
		if(element) {
			element->update();
		}
	}
}

void GUIMaster::add(std::shared_ptr<GUIElement> element) {
	_elements.push_back(element);
}

void GUIMaster::draw(int mode) {
	GUIDrawElement::draw(mode, get_viewport());
	for(const auto element : _elements) {
		if(element) {
			element->draw(mode, get_viewport());
		}
	}
}

bool GUIMaster::selected() {
	for (const auto element : _elements) {
		if (element && element->selected()) {
			return true;
		}
	}
	return false;
}