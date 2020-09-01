#include "GUI.h"

#include "../src/System/Environment.h"
#include "../src/System/InputManager.h"
#include "../src/System/ResourceManager.h"
#include "../src/Resources/Program.h"
#include "../src/Resources/Window.h"
#include <GL/gl3w.h>

#define GUI_SHADER 2

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

glm::vec4 GUIPositionElement::screen_space() {
	const float window_width = (float)Environment::get().get_window()->get_width();
	const float window_height = (float)Environment::get().get_window()->get_height();
	return { _position.x * window_width,
			 _position.y * window_height,
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

void GUIDrawElement::draw(GUIDrawDesc draw_desc) {  
	glBindVertexArray(_vao);
	glUseProgram(_program);

	glUniform4fv(glGetUniformLocation(_program, "screen_space"), 1, &draw_desc._screen_space[0]);
	glUniform4fv(glGetUniformLocation(_program, "color"), 1, &_color[0]);
	glUniform2fv(glGetUniformLocation(_program, "position"), 1, &get_draw_position(draw_desc)[0]);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glDrawArrays(draw_desc._mode, 0, _vertex_data.size());

	glDisable(GL_BLEND);
}

glm::vec2 GUIDrawElement::get_draw_position(GUIDrawDesc draw_desc) {
	return draw_desc._has_master ? glm::vec2(_position.x + draw_desc._master_position.x, _position.y + draw_desc._ypos + _height + draw_desc._scroll) : _position;
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

void GUISelectElement::select(GUISelectDesc select_desc) {
	const float window_width = (float)Environment::get().get_window()->get_width();
	const float window_height = (float)Environment::get().get_window()->get_height();
	const float xpos = (float)Environment::get().get_input_manager()->get_mouse_x();
	const float ypos = (float)Environment::get().get_input_manager()->get_mouse_y();
	const glm::vec2 position = glm::vec2(_position.x + select_desc._master_position.x, _position.y + select_desc._ypos + select_desc._master_height + select_desc._scroll);

	_mouse_x = xpos - (position.x * window_width);
	_mouse_y = ypos - (window_height - _height - position.y * window_height);

	_valid = _mouse_x <= _width * window_width &&
		_mouse_x >= 0 &&
		_mouse_y <= _height * window_height &&
		_mouse_y >= 0;
}

GUIScrollElement::GUIScrollElement() :
	_scroll			( 0.0f ),
	_max_scroll		( 0.0f ),
	_width			( 0.01f ),
	_height			( 0.03f)
{
	generate_vertex_data();
	create_vao();
}

GUIScrollElement::GUIScrollElement(glm::vec4 color, float width, float height) :
	_scroll			( 0.0f ),
	_max_scroll		( 0.0f ),
	_width			( width ),
	_height			( height ),
	_color			( color )
{
	generate_vertex_data();
	create_vao();
}

void GUIScrollElement::generate_vertex_data() {
	_vertex_data.reserve(6);
	_vertex_data.push_back(glm::vec2(0, 0));
	_vertex_data.push_back(glm::vec2(_width, 0));
	_vertex_data.push_back(glm::vec2(0, _height));
	_vertex_data.push_back(glm::vec2(0, _height));
	_vertex_data.push_back(glm::vec2(_width, 0));
	_vertex_data.push_back(glm::vec2(_width, _height));
}

void GUIScrollElement::create_vao() {
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

void GUIScrollElement::draw(GUIDrawDesc draw_desc) {
	glBindVertexArray(_vao);
	glUseProgram(_program);

	glUniform4fv(glGetUniformLocation(_program, "viewport"), 1, &draw_desc._screen_space[0]);
	glUniform4fv(glGetUniformLocation(_program, "color"), 1, &_color[0]);
	glUniform2fv(glGetUniformLocation(_program, "position"), 1, &get_draw_position(draw_desc)[0]);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(draw_desc._mode, 0, _vertex_data.size());

	glDisable(GL_BLEND);
}

glm::vec2 GUIScrollElement::get_draw_position(GUIDrawDesc draw_desc) {
	const float xpos = draw_desc._master_position.x + draw_desc._master_width - _width;
	const float scroll_ratio = draw_desc._scroll / (_max_scroll - GUIPositionElement::_height);
	const float scroll_distance = (draw_desc._master_height - _height) * scroll_ratio;
	const float ypos = draw_desc._master_position.y + GUIPositionElement::_height - _height - scroll_distance;

	return draw_desc._has_master ? glm::vec2(xpos, ypos) : _position;
}

void GUIScrollElement::scroll(double yoffset) {
	const float max = _max_scroll - GUIPositionElement::_height;
	if (max > 0) {
		_scroll -= (float)yoffset / 100.0f;
		_scroll = std::clamp(_scroll, 0.0f, max);
	}
}

GUISelectionGrid::GUISelectionGrid(float width, float height, glm::vec2 position, glm::vec4 color) :
	GUIPositionElement		( width, height, position, color )
{}

void GUISelectionGrid::select(GUISelectDesc select_desc) {
	GUISelectElement::select(select_desc);
}

void GUISelectionGrid::draw(GUIDrawDesc draw_desc) {
	GUIDrawElement::draw(draw_desc);
}

bool GUISelectionGrid::selected() {
	return GUISelectElement::selected();
}

GUIMaster::GUIMaster(float width, float height, glm::vec2 position, glm::vec4 color) :
	GUIPositionElement		( width, height, position, color ),
	GUIScrollElement		( glm::vec4(color.r, color.g, color.b, color.a + .2) )
{}

void GUIMaster::update() {
	GUISelectDesc const master_select_desc(0.0f, GUIPositionElement::_height);
	GUISelectDesc element_select_desc(GUIPositionElement::_width, GUIPositionElement::_height, GUIPositionElement::_position, _scroll);

	GUISelectElement::select(master_select_desc);
	for(const auto element : _elements) {
		element->select(element_select_desc);
		element_select_desc._ypos -= std::dynamic_pointer_cast<GUIPositionElement>(element)->_height;
	}
}

void GUIMaster::add(std::shared_ptr<GUIElement> element) {
	_elements.push_back(element);

	_max_scroll += std::dynamic_pointer_cast<GUIPositionElement>(element)->_height;
}

void GUIMaster::draw(int mode) {
	const auto screen_space = GUIPositionElement::screen_space();
    GUIDrawDesc const master_draw_desc(mode, screen_space);
	GUIDrawDesc element_draw_desc(mode, screen_space, GUIPositionElement::_width, GUIPositionElement::_height, GUIPositionElement::_position, true, _scroll);

	GUIDrawElement::draw(master_draw_desc);

	for(const auto element : _elements) {
		element->draw(element_draw_desc);
		element_draw_desc._ypos -= std::dynamic_pointer_cast<GUIPositionElement>(element)->_height;
	}

	if (_max_scroll - GUIPositionElement::_height > 0) {
		GUIScrollElement::draw(element_draw_desc);
	}
}

bool GUIMaster::selected() {
	return GUISelectElement::selected();
}