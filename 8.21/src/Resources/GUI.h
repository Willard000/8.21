#ifndef GUI_H
#define GUI_H

#include <glm/gtc/matrix_transform.hpp>

#include <vector>

typedef unsigned int GLuint;

class GUIElement {
public:
	GUIElement(int width, int height, glm::vec2 position, glm::vec4 color);

	void draw(int mode);
protected:
	int _width;
	int _height;
	glm::vec2 _position;
	glm::vec4 _color;
private:
	void create_vertices();
	void load_vao();
private:
	GLuint _vao;
	GLuint _program;
	GLuint _vertex_buffer;

	std::vector<glm::vec2> _vertex_data;
};

class GUISelectElement : public GUIElement {
public:
	void update();
private:
	int _mouse_x;
	int _mouse_y;
	bool _valid;
};

class GUISelectionGrid : public GUISelectElement {
public:
private:
};

#endif