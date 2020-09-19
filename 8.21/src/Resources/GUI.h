#ifndef GUI_H
#define GUI_H

#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <memory>
#include <string>

typedef unsigned int GLuint;

struct Texture;

const glm::mat4 GUI_PROJECTION = glm::ortho(0, 1, 0, 1);

/********************************************************************************************************************************************************/

struct GUIDrawDesc {
	int		  _mode = 4;
	float	  _width = 0.0f;
	float     _height = 0.0f;
	glm::vec2 _position = glm::vec2(0, 0);
	glm::vec4 _color = glm::vec4(0, 0, 0, 1);
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
	glm::vec4   _color = glm::vec4(1, 1, 1, 1);
	float       _scale = 0.15f;
	glm::vec2   _position = glm::vec2(0, 0);
	int         _mode = 4;
};

struct GUIIconDesc {
	int			_mode = 4;
	float		_width = 0.0f;
	float		_height = 0.0f;
	glm::vec2	_position = glm::vec2(0, 0);
	GLuint		_texture = 0;
	bool        _highlight = false;
};

/********************************************************************************************************************************************************/

class GUIElement {
public:
	virtual void select(GUIMasterDesc master_desc = GUIMasterDesc()) = 0;
	virtual void draw(GUIMasterDesc master_desc = GUIMasterDesc()) = 0;
	virtual bool selected() = 0;
	virtual void click(GUIMasterDesc master_desc = GUIMasterDesc()) = 0;
};

/********************************************************************************************************************************************************/

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

/********************************************************************************************************************************************************/

class GUIDrawElement {
public:
	GUIDrawElement();
	~GUIDrawElement();

	void draw(GUIDrawDesc draw_desc = GUIDrawDesc(), GUIMasterDesc master_desc = GUIMasterDesc());
private:
	void create_vao();

	GLuint _vao;
	GLuint _program;
	GLuint _vertex_buffer;
};

/********************************************************************************************************************************************************/

class GUISelectElement : virtual public GUIPositionElement {
public:
	GUISelectElement();
	GUISelectElement(float width, float height, glm::vec2 position, glm::vec4 color);

	void select(GUIMasterDesc master_desc = GUIMasterDesc());
	bool select_icon(GUIIconDesc icon_Desc, GUIMasterDesc master_desc);
	bool selected();
protected:
	float _mouse_x;
	float _mouse_y;
	bool _valid;
};

/********************************************************************************************************************************************************/

class GUIScrollElement : virtual public GUIPositionElement {
public:
	GUIScrollElement();
	GUIScrollElement(glm::vec4 color, float width = 0.01f, float height = 0.03f);

	void scroll(double yoffset);

	void draw(GUIMasterDesc master_desc);
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

/********************************************************************************************************************************************************/

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

/********************************************************************************************************************************************************/

class GUIDrawIcon {
public:
	GUIDrawIcon();
	~GUIDrawIcon();

	void draw(GUIIconDesc icon_desc, GUIMasterDesc master_desc);
private:
	void create_vao();
private:
	GLuint _vao;
	GLuint _vertex_buffer;
	GLuint _program;
};

/********************************************************************************************************************************************************/

struct ReadIconFile {
	ReadIconFile(const char* file_path);

	int _id = -1;
	std::string _type;
};

struct GUIIcon {
	GUIIcon();
	GUIIcon(int id, std::string type, std::shared_ptr<Texture> texture);
	GUIIcon(int key, const char* file_path);

	int						 _key;
	int						 _id;  //Entity ID
	std::string			  	 _type;
	std::shared_ptr<Texture> _texture;
private:
	void load_from_file(const char* file_path);
};

/********************************************************************************************************************************************************/

class GUIButtonIcon : virtual public GUIElement, public GUISelectElement {
public:
	GUIButtonIcon(std::shared_ptr<GUIIcon> icon, void (*function)(), float width, float height, glm::vec2 position, glm::vec4 color);

	virtual void select(GUIMasterDesc master_desc = GUIMasterDesc());
	virtual void draw(GUIMasterDesc master_desc = GUIMasterDesc());
	virtual bool selected();

	void click(GUIMasterDesc master_desc);
private:
	GUIIconDesc _icon;
	void (*_function)();
};

/********************************************************************************************************************************************************/

class Entity;

class GUISelectionView : virtual public GUIElement, public GUISelectElement {
public:
	GUISelectionView(float width, float height, glm::vec2 position, glm::vec4 color);

	virtual void select(GUIMasterDesc master_desc = GUIMasterDesc());
	virtual void draw(GUIMasterDesc master_desc = GUIMasterDesc());
	virtual bool selected();

	void click(GUIMasterDesc master_desc);

	void set_selection(std::string_view type, unsigned int id);
	void set_selection(std::shared_ptr<Entity> entity);

	void display_entity_info(GUIMasterDesc master_desc = GUIMasterDesc());
private:
	std::shared_ptr<Entity> _selection;
};

/********************************************************************************************************************************************************/

class GUISelectionGrid : virtual public GUIElement, public GUISelectElement {
public:
	GUISelectionGrid(float width, float height, glm::vec2 position, glm::vec4 color);

	virtual void select(GUIMasterDesc master_desc = GUIMasterDesc());
	virtual void draw(GUIMasterDesc master_desc = GUIMasterDesc());
	virtual bool selected();

	void click(GUIMasterDesc master_desc);

	void set_title(std::string_view title);
	void add(std::shared_ptr<GUIIcon> icon);
	std::shared_ptr<GUIIcon> get_selection();

	void set_view(std::shared_ptr<GUISelectionView> view);
private:
	GUIIconDesc _icon_desc;
	GUITextDesc _title;
	std::shared_ptr<GUIIcon> _selection;

	std::shared_ptr<GUISelectionView> _view;

	std::vector<std::shared_ptr<GUIIcon>> _icons;
};

/********************************************************************************************************************************************************/

class GUIMaster : virtual public GUISelectElement, virtual public GUIScrollElement {
public:
	GUIMaster(float width, float height, glm::vec2 position, glm::vec4 color);

	void update();
	void add(std::shared_ptr<GUIElement>);
	void draw(int mode);
	void click();
	bool selected();
private:
	std::vector<std::shared_ptr<GUIElement>> _elements;
};

#endif