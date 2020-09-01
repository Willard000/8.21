#ifndef GUI_TEXT_H
#define GUI_TEXT_H

typedef unsigned int GLuint;

#include <glm/gtc/matrix_transform.hpp>

struct GUITextDrawDesc {
	GUITextDrawDesc(char c, glm::vec4 color = glm::vec4(1, 1, 1, 1), float scale = 0.1f, glm::vec2 position = glm::vec2(0.0f, 0.0f)) :
		_char				( c ),
		_color				( color ),
		_scale				( scale ),
		_position			( position )
	{}

	char _char;
	glm::vec4 _color;
	float _scale;
	glm::vec2 _position;
};

class GUITextDrawElement {
public:
	GUITextDrawElement();

	void draw(int mode, GUITextDrawDesc text_draw_desc);
private:
	void create_vao();
	void load_font_atlas();

	GLuint _vao;
	GLuint _program;
	GLuint _vertex_buffer;

	GLuint _font_atlas;
};

class GUIText {
public:
private:
};

#endif