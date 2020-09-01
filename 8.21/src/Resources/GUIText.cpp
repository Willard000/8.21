#include "GUIText.h"

#include <GL/gl3w.h>

#include "../src/System/Environment.h"
#include "../src/System/ResourceManager.h"
#include "../src/Resources/Program.h"
#include "../src/Resources/GUI.h"

#include "../src/Resources/FontMap.h"

#include <SOIL/SOIL2.h>

#define GUI_TEXT_SHADER 3
#define VERDANA_FONT_PATH "Data\\Font\\verdana.png"

constexpr GLfloat vertex_data[12] = { 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1 };

static FontMap font_map;

GUITextDrawElement::GUITextDrawElement() {
	create_vao();
	load_font_atlas();
}

void GUITextDrawElement::create_vao() {
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

void GUITextDrawElement::load_font_atlas() {
	_font_atlas = SOIL_load_OGL_texture(VERDANA_FONT_PATH, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void GUITextDrawElement::draw(int mode, GUITextDrawDesc text_draw_desc) {
	glBindVertexArray(_vao);
	glUseProgram(_program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _font_atlas);

	const auto width = font_map.width(text_draw_desc._char);
	const auto height = font_map.height(text_draw_desc._char);
	const auto position = font_map.position(text_draw_desc._char);

	glUniform1f(glGetUniformLocation(_program, "scale"), text_draw_desc._scale);
	glUniform4fv(glGetUniformLocation(_program, "color"), 1, &text_draw_desc._color[0]);
	glUniform2fv(glGetUniformLocation(_program, "position"), 1, &text_draw_desc._position[0]);
	glUniform1f(glGetUniformLocation(_program, "text_width"), font_map.width(text_draw_desc._char));
	glUniform1f(glGetUniformLocation(_program, "text_height"), font_map.height(text_draw_desc._char));
	glUniform2fv(glGetUniformLocation(_program, "text_position"), 1, &font_map.position(text_draw_desc._char)[0]);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(mode, 0, 6);

	glDisable(GL_BLEND);
}