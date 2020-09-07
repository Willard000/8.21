#ifndef ICON_H
#define ICON_H

#include "../src/Resources/Texture.h"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>

struct GUIIconDesc {
	int			_mode = 4;
	float		_width = 0.0f;
	float		_height = 0.0f;
	glm::vec2	_position = glm::vec2(0, 0);
	GLuint		_texture;
};

class GUIDrawIcon {
public:
	GUIDrawIcon();
	~GUIDrawIcon();

	void draw(GUIIconDesc icon_desc);
private:
	void create_vao();
private:
	GLuint _vao;
	GLuint _vertex_buffer;
	GLuint _program;
};

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

#endif