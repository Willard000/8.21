#ifndef GUI_H
#define GUI_H

#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <memory>
#include <string>

#include "../src/Resources/Icon.h"

typedef unsigned int GLuint;

const glm::mat4 GUI_PROJECTION = glm::ortho(0, 1, 0, 1);

struct GUIDrawDesc {
	int		  _mode = 4;
};

struct GUIMasterDesc {
	glm::vec4 _screen_space = glm::vec4(0, 0, 0, 0);
	float     _width = 0.0f;
	float	  _height = 0.0f;
	glm::vec2 _position = glm::vec2(0, 0);
	float	  _ypos = 0.0f;
	float	  _scroll = 0.0f;
	float	  _sub_height = 0.0f;
};

struct GUITextDesc {
	std::string _string;
	glm::vec4 _color = glm::vec4(1, 1, 1, 1);
	float _scale = 0.15f;
	glm::vec2 _position = glm::vec2(0, 0);
	int _mode = 4;
};

class GUIElement {
public:
	virtual void select(GUIMasterDesc master_desc = GUIMasterDesc()) = 0;
	virtual void draw(GUIDrawDesc draw_desc = GUIDrawDesc(), GUIMasterDesc master_desc = GUIMasterDesc()) = 0;
	virtual bool selected() = 0;
};


struct GUIPositionElement {
public:
	GUIPositionElement();
	GUIPositionElement(float width, float height, glm::vec2 position, glm::vec4 color);

	glm::vec4 screen_space();

	float _width;
	float _height;
	glm::vec2 _position;
	glm::vec4 _color;
};

class GUIDrawElement : virtual public GUIPositionElement {
public:
	GUIDrawElement();
	GUIDrawElement(float width, float height, glm::vec2 position, glm::vec4 color);
	~GUIDrawElement();

	void draw(GUIDrawDesc draw_desc = GUIDrawDesc(), GUIMasterDesc master_desc = GUIMasterDesc());
private:
	void generate_vertex_data();
	void create_vao();

	GLuint _vao;
	GLuint _program;
	GLuint _vertex_buffer;

	std::vector<glm::vec2> _vertex_data;
};

class GUISelectElement : virtual public GUIPositionElement {
public:
	GUISelectElement();
	GUISelectElement(float width, float height, glm::vec2 position, glm::vec4 color);

	void select(GUIMasterDesc master_desc = GUIMasterDesc());
	bool selected();
private:
	float _mouse_x;
	float _mouse_y;
	bool _valid;
};

class GUIScrollElement : virtual public GUIPositionElement {
public:
	GUIScrollElement();
	GUIScrollElement(glm::vec4 color, float width = 0.01f, float height = 0.03f);

	void scroll(double yoffset);

	void draw(GUIDrawDesc draw_desc, GUIMasterDesc master_desc);
protected:
	float _scroll;
	float _max_scroll;
	float _width;
	float _height;

	glm::vec4 _color;
private:
	void generate_vertex_data();
	void create_vao();

	GLuint _vao;
	GLuint _program;
	GLuint _vertex_buffer;

	std::vector<glm::vec2> _vertex_data;
};

class GUIDrawText {
public:
	GUIDrawText();
	~GUIDrawText();

	void draw(GUITextDesc text_desc, GUIMasterDesc master_desc);
private:
	void create_vao();
	void load_font_atlas();

	GLuint _vao;
	GLuint _program;
	GLuint _vertex_buffer;

	GLuint _font_atlas;
};

class GUISelectionGrid : virtual public GUIElement, public GUIDrawElement, public GUISelectElement {
public:
	GUISelectionGrid(float width, float height, glm::vec2 position, glm::vec4 color);

	virtual void select(GUIMasterDesc master_desc = GUIMasterDesc());
	virtual void draw(GUIDrawDesc draw_desc = GUIDrawDesc(), GUIMasterDesc master_desc = GUIMasterDesc());
	virtual bool selected();

	void add(std::shared_ptr<GUIIcon> icon);
private:
	GUITextDesc _title;

	std::vector<std::shared_ptr<GUIIcon>> _icons;
};

class GUIMaster : virtual public GUIDrawElement, virtual public GUISelectElement, virtual public GUIScrollElement {
public:
	GUIMaster(float width, float height, glm::vec2 position, glm::vec4 color);

	void update();
	void add(std::shared_ptr<GUIElement>);
	void draw(int mode);
	bool selected();
private:
	std::vector<std::shared_ptr<GUIElement>> _elements;
};

#endif