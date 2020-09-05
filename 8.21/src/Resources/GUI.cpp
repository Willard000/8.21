#include "GUI.h"

#include "../src/System/Environment.h"
#include "../src/System/InputManager.h"
#include "../src/System/ResourceManager.h"
#include "../src/System/GUIManager.h"
#include "../src/Resources/Program.h"
#include "../src/Resources/Window.h"

#include "../src/Resources/FontMap.h"

#include <SOIL/SOIL2.h>

#include <GL/gl3w.h>

#define GUI_SHADER 2
#define GUI_TEXT_SHADER 3
#define VERDANA_FONT_PATH "Data\\Font\\verdana.png"

constexpr GLfloat vertex_data[12] = { 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1 };

static FontMap font_map;

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

void GUIDrawElement::draw(GUIDrawDesc draw_desc, GUIMasterDesc master_desc) {  
	glBindVertexArray(_vao);
	glUseProgram(_program);

	const bool child_element = master_desc._width + master_desc._height;										// If attached to a master gui element
	const auto position = glm::vec2(
						  _position.x + master_desc._position.x,
						  _position.y + master_desc._ypos + (child_element * _height) + master_desc._scroll		// add height
	);	

	glUniform4fv(glGetUniformLocation(_program, "screen_space"), 1, &master_desc._screen_space[0]);
	glUniform4fv(glGetUniformLocation(_program, "color"), 1, &_color[0]);
	glUniform2fv(glGetUniformLocation(_program, "position"), 1, &position[0]);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glDrawArrays(draw_desc._mode, 0, _vertex_data.size());

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

void GUISelectElement::select(GUIMasterDesc master_desc) {
	const float window_width = (float)Environment::get().get_window()->get_width();
	const float window_height = (float)Environment::get().get_window()->get_height();
	const float xpos = (float)Environment::get().get_input_manager()->get_mouse_x();
	const float ypos = (float)Environment::get().get_input_manager()->get_mouse_y();
	const auto position = glm::vec2(_position.x + master_desc._position.x, _position.y + master_desc._ypos + master_desc._height + master_desc._scroll);

	_mouse_x = xpos - (position.x * window_width);
	_mouse_y = ypos - (window_height - _height - position.y * window_height);

	_valid = _mouse_x <= _width * window_width   &&
			 _mouse_y <= _height * window_height &&
			 _mouse_x >= 0						 &&
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

void GUIScrollElement::draw(GUIDrawDesc draw_desc, GUIMasterDesc master_desc) {
	glBindVertexArray(_vao);
	glUseProgram(_program);

	const float xpos = master_desc._position.x + master_desc._width - _width;
	const float ratio = master_desc._scroll / (_max_scroll - GUIPositionElement::_height);
	const float distance = (master_desc._height - _height) * ratio;
	const float ypos = master_desc._position.y + GUIPositionElement::_height - _height - distance;
	const auto  position = glm::vec2(xpos, ypos);

	glUniform4fv(glGetUniformLocation(_program, "viewport"), 1, &master_desc._screen_space[0]);
	glUniform4fv(glGetUniformLocation(_program, "color"), 1, &_color[0]);
	glUniform2fv(glGetUniformLocation(_program, "position"), 1, &position[0]);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(draw_desc._mode, 0, _vertex_data.size());

	glDisable(GL_BLEND);
}

void GUIScrollElement::scroll(double yoffset) {
	const float max = _max_scroll - GUIPositionElement::_height;
	if (max > 0) {
		_scroll -= (float)yoffset / 100.0f;
		_scroll = std::clamp(_scroll, 0.0f, max);
	}
}

GUIDrawText::GUIDrawText() {
	create_vao();
	load_font_atlas();
}

void GUIDrawText::create_vao() {
	glCreateVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	_program = Environment::get().get_resource_manager()->get_program(GUI_TEXT_SHADER)->_id;
	glUseProgram(_program);

	glUniformMatrix4fv(glGetUniformLocation(_program, "projection"), 1, GL_FALSE, &GUI_PROJECTION[0][0]);

	glCreateBuffers(1, &_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
	glNamedBufferStorage(_vertex_buffer, sizeof(GLfloat) * 12, vertex_data, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void GUIDrawText::load_font_atlas() {
	_font_atlas = SOIL_load_OGL_texture(VERDANA_FONT_PATH, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void GUIDrawText::draw(GUITextDesc text_desc, GUIMasterDesc master_desc) {
	if (text_desc._string.empty()) {
		return;
	}

	glBindVertexArray(_vao);
	glUseProgram(_program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _font_atlas);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const bool child_element = master_desc._width + master_desc._height;

	if (child_element) {
		text_desc._position += master_desc._position;
		text_desc._position.y += master_desc._scroll;
	}

	for (const auto character : text_desc._string) {
		const auto text_width = font_map.width(character);

		glUniform4fv(glGetUniformLocation(_program, "screen_space"), 1, &master_desc._screen_space[0]);
		glUniform1f(glGetUniformLocation(_program, "scale"), text_desc._scale);
		glUniform4fv(glGetUniformLocation(_program, "color"), 1, &text_desc._color[0]);
		glUniform2fv(glGetUniformLocation(_program, "position"), 1, &text_desc._position[0]);
		glUniform1f(glGetUniformLocation(_program, "text_width"), text_width);
		glUniform1f(glGetUniformLocation(_program, "text_height"), font_map.height(character));
		glUniform2fv(glGetUniformLocation(_program, "text_position"), 1, &font_map.position(character)[0]);

		glDrawArrays(text_desc._mode, 0, 6);

		text_desc._position.x += text_width * text_desc._scale;
	}

	glDisable(GL_BLEND);
}

GUISelectionGrid::GUISelectionGrid(float width, float height, glm::vec2 position, glm::vec4 color) :
	GUIPositionElement		( width, height, position, color )
{
	_title._string = "Title";
}

void GUISelectionGrid::select(GUIMasterDesc master_desc) {
	GUISelectElement::select(master_desc);
}

void GUISelectionGrid::draw(GUIDrawDesc draw_desc, GUIMasterDesc master_desc) {
	GUIDrawElement::draw(draw_desc, master_desc);

	master_desc._position.y += _height;
	Environment::get().get_gui_manager()->draw_text(_title, master_desc);
}

bool GUISelectionGrid::selected() {
	return GUISelectElement::selected();
}

GUIMaster::GUIMaster(float width, float height, glm::vec2 position, glm::vec4 color) :
	GUIPositionElement		( width, height, position, color ),
	GUIScrollElement		( glm::vec4(color.r, color.g, color.b, color.a + .2) )
{}

void GUIMaster::update() {
	GUIMasterDesc master_desc;
	master_desc._width = 0.0f;
	master_desc._height = GUIPositionElement::_height;

	GUIMasterDesc element_master_desc;
	element_master_desc._width = GUIPositionElement::_width;
	element_master_desc._height = GUIPositionElement::_height;
	element_master_desc._position = GUIPositionElement::_position;
	element_master_desc._scroll = _scroll;

	GUISelectElement::select(master_desc);
	for(const auto element : _elements) {
		element->select(element_master_desc);
		element_master_desc._ypos -= std::dynamic_pointer_cast<GUIPositionElement>(element)->_height;
	}
}

void GUIMaster::add(std::shared_ptr<GUIElement> element) {
	_elements.push_back(element);

	_max_scroll += std::dynamic_pointer_cast<GUIPositionElement>(element)->_height;
}

void GUIMaster::draw(int mode) {
	const auto screen_space = GUIPositionElement::screen_space();
	GUIDrawDesc draw_desc;
	draw_desc._mode = mode;

	GUIMasterDesc master_desc;
	master_desc._screen_space = screen_space;

	GUIMasterDesc element_master_desc;
	element_master_desc._screen_space = screen_space;
	element_master_desc._width = GUIPositionElement::_width;
	element_master_desc._height = GUIPositionElement::_height;
	element_master_desc._position = GUIPositionElement::_position;
	element_master_desc._ypos = GUIPositionElement::_position.y;
	element_master_desc._scroll = _scroll;

	GUIDrawElement::draw(draw_desc, master_desc);

	for(const auto element : _elements) {
		element->draw(draw_desc, element_master_desc);
		element_master_desc._ypos -= std::dynamic_pointer_cast<GUIPositionElement>(element)->_height;
	}

	if (_max_scroll - GUIPositionElement::_height > 0) {
		GUIScrollElement::draw(draw_desc, element_master_desc);
	}
}

bool GUIMaster::selected() {
	return GUISelectElement::selected();
}

