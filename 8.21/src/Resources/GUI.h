#ifndef GUI_H
#define GUI_H

#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <memory>

typedef unsigned int GLuint;

class GUIViewPort;

class GUIElement {
public:
	virtual void update() = 0;
	virtual void draw(int mode, glm::vec4 viewport) = 0;
	virtual bool selected() = 0;
private:
};

class GUIPositionElement {
public:
	GUIPositionElement();
	GUIPositionElement(float width, float height, glm::vec2 position, glm::vec4 color);
protected:
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

	void draw(int mode, glm::vec4 viewport);
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

	void update();
	bool selected();
private:
	int _mouse_x;
	int _mouse_y;
	bool _valid;
};

class GUISelectionGrid : public GUIElement, public GUISelectElement, public GUIDrawElement {
public:
	GUISelectionGrid(float width, float height, glm::vec2 position, glm::vec4 color);

	void update();
	void draw(int mode, glm::vec4 viewport);
	bool selected();
private:
};

class GUIMaster : public GUIViewPort, virtual public GUIDrawElement {
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