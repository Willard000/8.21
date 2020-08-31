#ifndef GUI_H
#define GUI_H

#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <memory>

typedef unsigned int GLuint;

struct GUIDrawDesc {
	GUIDrawDesc(int mode = 4, glm::vec4 viewport = glm::vec4(0, 0, 0, 0), float master_width = 0.0f, float master_height = 0.0f, glm::vec2 master_position = glm::vec2(0, 0), bool has_master = false, float scroll = 0.0f) :
		_mode					( mode ),
		_viewport				( viewport ),
		_master_width			( master_width ),
		_master_height			( master_height ),
		_master_position		( master_position ),
		_has_master				( has_master ),
		_ypos					( master_position.y ),
		_scroll					( scroll )
	{}

	int		  _mode;
	glm::vec4 _viewport;
	float	  _master_width;
	float	  _master_height;
	glm::vec2 _master_position;
	bool	  _has_master;
	float	  _ypos;
	float	  _scroll;
};

struct GUISelectDesc {
	GUISelectDesc(float master_width = 0.0f, float master_height = 0.0f, glm::vec2 master_position = glm::vec2(0, 0), float scroll = 0.0f) :
		_master_width			( master_width ),
		_master_height			( master_height ),
		_master_position		( master_position ),
		_ypos					( master_position.y ),
		_scroll					( scroll )
	{}

	float		_master_width;
	float		_master_height;
	glm::vec2   _master_position;
	float		_ypos;
	float		_scroll;
};

class GUIViewPort;
class GUIMaster;  

class GUIElement {
public:
	virtual void select(GUISelectDesc select_desc = GUISelectDesc()) = 0;
	virtual void draw(GUIDrawDesc draw_desc = GUIDrawDesc()) = 0;
	virtual bool selected() = 0;
};


struct GUIPositionElement {
public:
	GUIPositionElement();
	GUIPositionElement(float width, float height, glm::vec2 position, glm::vec4 color);

	float _width;
	float _height;
	glm::vec2 _position;
	glm::vec4 _color;
};

class GUIViewPort : virtual public GUIPositionElement {
public:
	GUIViewPort();
	GUIViewPort(float width, float height);

	glm::vec4 get_viewport();
protected:
	glm::vec2 _position;
	float _width;
	float _height;
};

class GUIDrawElement : virtual public GUIPositionElement {
public:
	GUIDrawElement();
	GUIDrawElement(float width, float height, glm::vec2 position, glm::vec4 color);
	~GUIDrawElement();

	void draw(GUIDrawDesc draw_desc = GUIDrawDesc());
	glm::vec2 get_draw_position(GUIDrawDesc draw_desc);
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

	void select(GUISelectDesc select_desc = GUISelectDesc());
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

	void draw(GUIDrawDesc draw_desc);
	glm::vec2 get_draw_position(GUIDrawDesc draw_desc);
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

class GUISelectionGrid : virtual public GUIElement, public GUIDrawElement, public GUISelectElement {
public:
	GUISelectionGrid(float width, float height, glm::vec2 position, glm::vec4 color);

	virtual void select(GUISelectDesc select_desc = GUISelectDesc());
	virtual void draw(GUIDrawDesc draw_desc = GUIDrawDesc());
	virtual bool selected();
private:
};

class GUIMaster : public GUIViewPort, virtual public GUIDrawElement, virtual public GUISelectElement, virtual public GUIScrollElement {
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