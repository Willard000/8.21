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

	GLfloat min_height() {
		GLfloat min = height[0];
		for(unsigned int i = 1; i < 4; ++i) {
			if(height[i] < min) {
				min = height[i];
			}
		}
		return min;
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

enum {
	HEIGHT_TOP, HEIGHT_BOTTOM, HEIGHT_LEFT, HEIGHT_RIGHT, HEIGHT_TOP_LEFT, HEIGHT_BOTTOM_LEFT, HEIGHT_TOP_RIGHT, HEIGHT_BOTTOM_RIGHT, HEIGHT_CENTER
};

class TileSelection : virtual public TerrainData {
public:
	TileSelection();

	void select(int x, int z);
	bool select(glm::vec3 world_space, glm::vec3 position);

	void draw();
	bool valid_index(int index);
	bool valid_left_index(int index);
	bool valid_right_index(int index);
	void update_vao();

	bool is_valid_tile();

	bool above_terrain(glm::vec3 world_space, glm::vec3 position, float height);
	float exact_height(float x, float z);

	glm::vec2 get_selected_tile();

	void entity_tile_index(float x, float z);
	void entity_height_case(int x_height_case, int z_height_case);
	float entity_tile_height(int height_case);
	float max_tile_height(int i, int i2, int i3, int i4);
protected:
	int _x;
	int _z;

	int _index;

	bool _valid_index;

	int _entity_x_index;
	int _entity_z_index;
	int _entity_tile_index;
	int _entity_height_case;
private:
	void create_vao();
private:
	std::vector<glm::vec3> _vertex_data;

	GLuint _vao;
	GLuint _program;
	GLuint _vertex_buffer;

	glm::vec4 _color;

	Transform _transform;
};

/********************************************************************************************************************************************************/

class TerrainEntities : virtual public TileSelection, virtual public TerrainData {
public:
	TerrainEntities();

	void add_entity(std::shared_ptr<Entity> entity);
	std::shared_ptr<Entity> remove_entity();
	std::shared_ptr<Entity> get_entity();
	void adjust_entity_height();

	std::shared_ptr<Entity> select_entity(glm::vec3 world_space, glm::vec3 position);
	bool above_entity(glm::vec3 world_space, glm::vec3 position, float height);
	float entity_height(int x, int z);
	glm::vec3 entity_placement();

	bool is_empty_tile();
protected:
	std::vector<std::shared_ptr<Entity>> _entities;
};

/********************************************************************************************************************************************************/

class Terrain : virtual public TileSelection, public TerrainEntities, virtual public TerrainData {
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