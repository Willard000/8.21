#ifndef TERRAIN_H
#define TERRAIN_H

#include <glm/gtc/matrix_transform.hpp>
#include <GL/gl3w.h>

#include <vector>
#include <memory>

#include "Texture.h"
#include "Transform.h"
#include "../src/Entities/Entity.h"

/********************************************************************************************************************************************************/

struct TileHeight {
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

/********************************************************************************************************************************************************/

class TerrainData {
public:
	TerrainData();
	TerrainData(int width, int height, float tile_width, float tile_length);
protected:
	int _width;
	int _length;
	float _tile_width;
	float _tile_length;

	std::vector<TileHeight> _height_map;
};

/********************************************************************************************************************************************************/

class TileSelection : virtual public TerrainData {
public:
	TileSelection();

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

/********************************************************************************************************************************************************/

class TerrainEntities : virtual public TerrainData {
public:
	TerrainEntities();

	void add_entity(std::shared_ptr<Entity> entity, int tile_x, int tile_z);
	void remove_entity(int tile_x, int tile_z);
	void adjust_entity_height(int tile_x, int tile_z);
protected:
	std::vector<std::shared_ptr<Entity>> _entities;
};

/********************************************************************************************************************************************************/

class Terrain : public TileSelection, public TerrainEntities, virtual public TerrainData {
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
	TileHeight get_tile_height(int x, int z);
	float get_vertex_height(int index, int vertex);

	TileSelection* get_tile_selection();
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

/********************************************************************************************************************************************************/

#endif