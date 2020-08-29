#include "GUI.h"

#include "../src/System/Environment.h"
#include "../src/System/InputManager.h"
#include "../src/System/ResourceManager.h"
#include "../src/Resources/Program.h"
#include "../src/Resources/Window.h"

#include <GL/gl3w.h>

#define GUI_SHADER 2

const glm::mat4 GUI_PROJECTION = glm::ortho(0, 1, 0, 1);

// width, height position 0.0f - 1.0f ratio
GUIElement::GUIElement(int width, int height, glm::vec2 position, glm::vec4 color) :
	_width			( width ),
	_height			( height ),
	_position		( position ),
	_color			( color )
{
	create_vertices();
	load_vao();
}

void GUIElement::draw(int mode) {
	glBindVertexArray(_vao);
	glUseProgram(_program);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glDrawArrays(mode, 0, _vertex_data.size());

	glDisable(GL_BLEND);
}

void GUIElement::create_vertices() {
	float normalized_width = (float)_width / (float)Environment::get().get_window()->get_width();
	float normalized_height = (float)_height / (float)Environment::get().get_window()->get_height();
	_vertex_data.reserve(6);
	_vertex_data.push_back(glm::vec2(0, 0));
	_vertex_data.push_back(glm::vec2(normalized_width, 0));
	_vertex_data.push_back(glm::vec2(0, normalized_height));
	_vertex_data.push_back(glm::vec2(0, normalized_height));
	_vertex_data.push_back(glm::vec2(normalized_width, 0));
	_vertex_data.push_back(glm::vec2(normalized_width, normalized_height));
}

void GUIElement::load_vao() {
	glCreateVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	_program = Environment::get().get_resource_manager()->get_program(GUI_SHADER)->_id;
	glUseProgram(_program);
	glUniformMatrix4fv(glGetUniformLocation(_program, "projection"), 1, GL_FALSE, &GUI_PROJECTION[0][0]);
	glUniform4fv(glGetUniformLocation(_program, "color"), 1, &_color[0]);
	glUniform2fv(glGetUniformLocation(_program, "position"), 1, &_position[0]);

	glCreateBuffers(1, &_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
	glNamedBufferStorage(_vertex_buffer, sizeof(glm::vec2) * _vertex_data.size(), &_vertex_data[0], 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void GUISelectElement::update() {
	_mouse_x = int(Environment::get().get_input_manager()->get_mouse_x() - _position.x);
	_mouse_y = int(Environment::get().get_input_manager()->get_mouse_y() - _position.y);
	_valid = _mouse_x <= _width && _mouse_x >= _position.x && _mouse_y <= _height && _mouse_y >= _position.y;
}