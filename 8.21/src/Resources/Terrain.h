#ifndef TERRAIN_H
#define TERRAIN_H

#include <glm/gtc/matrix_transform.hpp>
#include <GL/gl3w.h>

#include <vector>
#include <memory>

#include "Texture.h"
#include "Transform.h"

struct Tile_Height {
	//   0-------1
	//   | tile  |
	//   |       |
	//   2-------3
	GLfloat height[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	bool is_flat() {
		for(unsigned int i = 1; i < 4; ++i) {
			if(!(height[0] == height[i])) {
				return false;
			}
		}
		return true;
	}
};

class Terrain_Data {
public:
	Terrain_Data(int width, int height, float tile_width, float tile_length);
protected:
	int _width;
	int _length;
	float _tile_width;
	float _tile_length;

	std::vector<Tile_Height> _height_map;
};

class Tile_Selection : public Terrain_Data {
public:
	Tile_Selection(int width, int height, float tile_width, float tile_length);

	void select(int x, int z);
	void draw();
	bool valid_index(int index);
	bool valid_left_index(int index);
	bool valid_right_index(int index);
	void update_vao();
protected:
	int _x;
	int _z;
	int _index;

	bool _valid_index;
private:
	void create_vao();
private:
	std::vector<glm::vec3> _vertex_data;

	GLuint _vao;
	GLuint _program;
	GLuint _vertex_buffer;

	Transform _transform;
};

class Terrain : public Tile_Selection {
public:
	Terrain(int width, int length, float tile_width, float tile_length);
	~Terrain();

	void draw(int mode);
	void adjust_tile_height(float height);
	void adjust_ramp_height();
	void adjust_front_ramp();
	void adjust_back_ramp();
	void adjust_right_ramp();
	void adjust_left_ramp();

	void adjust_vertex_height(int index, int vertex, float height);

	float get_tile_width();
	float get_tile_length();
	Tile_Height get_tile_height(int x, int z);
	float get_vertex_height(int index, int vertex);

	Tile_Selection* get_tile_selection();
private:
	void generate_vertex_data();
	void generate_uv_data();
	void generate_position_data();
	void load_textures();
	void create_vao();
private:
	std::vector<glm::vec2> _vertex_data;
	std::vector<glm::vec2> _uv_data;
	std::vector<glm::vec2> _position_data;

	GLuint _vao;
	GLuint _program;
	GLuint _vertex_buffer;
	GLuint _uv_buffer;
	GLuint _position_buffer;
	GLuint _height_buffer;
	GLuint _height_texture;

	Texture _tile_texture;
};

#endif