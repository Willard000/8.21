#include "Terrain.h"

#include <SOIL/SOIL2.h>

#include "../src/System/Environment.h"
#include "../src/System/ResourceManager.h"
#include "../src/Resources/Program.h"

#include <iostream>
#include <sstream>

#define TERRAIN_SHADER_ID 1
#define TILE_SELECITON_SHADER_ID 7

/********************************************************************************************************************************************************/

TerrainData::TerrainData(int width, int length, float tile_width, float tile_length) :
	_width				( width ),
	_length				( length ),
	_tile_width			( tile_width ),
	_tile_length		( tile_length )
{
	_height_map.resize(width * length);
}

TerrainData::TerrainData(TerrainData&& terrain_data) noexcept :
	_width				( std::move(terrain_data._width) ),
	_length				( std::move(terrain_data._length) ),
	_tile_width			( std::move(terrain_data._tile_width) ),
	_tile_length		( std::move(terrain_data._tile_length) ),
	_height_map			( std::move(terrain_data._height_map) )
{}

TerrainData::TerrainData() :
	_width				( 0 ),
	_length				( 0 ),
	_tile_width			( 0 ),
	_tile_length		( 0 )
{}

void TerrainData::save(std::ofstream& file) {
	file << "# Terrain" << '\n';
	file << "width " << _width << '\n'
		 << "length " << _length << '\n'
		 << "tile_width " << _tile_width << '\n'
		 << "tile_length " << _tile_length << '\n';

	file << "height_map ";
	for(const auto tile : _height_map) {
		file << tile.height[0] << ' ' << tile.height[1] << ' ' << tile.height[2] << ' ' << tile.height[3] << ' ';
	}
	file << '\n';
}

void TerrainData::load(FileReader& file) {
	file.set_section("Terrain");
	file.read(&_width, "width");
	file.read(&_length, "length");
	file.read(&_tile_width, "tile_width");
	file.read(&_tile_length, "tile_length");

	_height_map.resize(_width * _length);

	std::string height_map;
	file.read(&height_map, "height_map");
	std::stringstream ss_height_map(height_map);

	for(int i = 0; i < _height_map.size(); ++i) {
		ss_height_map >> _height_map[i].height[0] >> _height_map[i].height[1] >> _height_map[i].height[2] >> _height_map[i].height[3];
	}
}

/********************************************************************************************************************************************************/

TileSelection::TileSelection() :
	_x				 ( 0 ),
	_z				 ( 0 ),
	_index			 ( 0 ),
	_valid_index	 ( false ),
	_color			 ( glm::vec4(1, 0, 0, .5) )
{
	_vertex_data.reserve(6);
	_vertex_data.push_back(glm::vec3(_tile_width, 0.01f, 0.0f));
	_vertex_data.push_back(glm::vec3(0.0f, 0.01f, 0.0f));
	_vertex_data.push_back(glm::vec3(0.0f, 0.01f, _tile_length));
	_vertex_data.push_back(glm::vec3(_tile_width, 0.01f, 0.0f));
	_vertex_data.push_back(glm::vec3(0.0f, 0.01f, _tile_length));
	_vertex_data.push_back(glm::vec3(_tile_width, 0.01f, _tile_length));

	create_vao();
}

TileSelection::~TileSelection() {
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vertex_buffer);
}

void TileSelection::create_vao() {
	glCreateVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	_program = Environment::get().get_resource_manager()->get_program(TILE_SELECITON_SHADER_ID)->_id;
	glUseProgram(_program);

	glUniformMatrix4fv(glGetUniformLocation(_program, "model"), 1, GL_FALSE, &_transform.get_model()[0][0]);

	glCreateBuffers(1, &_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
	glNamedBufferStorage(_vertex_buffer, sizeof(glm::vec3) * _vertex_data.size(), &_vertex_data[0], GL_DYNAMIC_STORAGE_BIT);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void TileSelection::update_vao() {
	glBindVertexArray(_vao);

	_transform.set_position(glm::vec3(_x * _tile_width, 0.f, _z * _tile_length));
	glUseProgram(_program);
	glUniformMatrix4fv(glGetUniformLocation(_program, "model"), 1, GL_FALSE, &_transform.get_model()[0][0]);

	_vertex_data[0].y = _height_map[_index].height[1] + 0.01f;;
	_vertex_data[1].y = _height_map[_index].height[0] + 0.01f;;
	_vertex_data[2].y = _height_map[_index].height[2] + 0.01f;;
	_vertex_data[3].y = _height_map[_index].height[1] + 0.01f;;
	_vertex_data[4].y = _height_map[_index].height[2] + 0.01f;;
	_vertex_data[5].y = _height_map[_index].height[3] + 0.01f;;

	glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * _vertex_data.size(), &_vertex_data[0]);
}

void TileSelection::select(int x, int z) {
	if ((x == _x && z == _z)) {
		return;
	}

	if (x < 0 || x > _width - 1 ||
		z < 0 || z > _length - 1) {
		_valid_index = false;
		return;
	}

	_x = x;
	_z = z;
	_index = _z * _width + _x;
	_valid_index = true;

	update_vao();
}

void TileSelection::draw() {
	glBindVertexArray(_vao);
	glUseProgram(_program);

	glUniformMatrix4fv(glGetUniformLocation(_program, "model"), 1, GL_FALSE, &_transform.get_model()[0][0]);
	glUniform4fv(glGetUniformLocation(_program, "color"), 1, &_color[0]);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_TRIANGLES, 0, _vertex_data.size());

	glDisable(GL_BLEND);
}

bool TileSelection::valid_index(int index) {
	return (index >= 0 && index < _width * _length);
}

bool TileSelection::valid_left_index(int index) {
	return (index / _width == (index + 1) / _width);
}

bool TileSelection::valid_right_index(int index) {
	return (index / _width == (index - 1) / _width);
}

bool TileSelection::is_valid_tile() {
	return _valid_index;
}

bool TileSelection::select(glm::vec3 world_space, glm::vec3 position) {
	float height = position.y;
	const float increment = height > 1.0f ? height * 0.01f : 0.01f;

	while (above_terrain(world_space, position, height)) {
		height -= increment;
	}

	const float y = abs((position.y - height) / world_space.y);
	const float x = (y * world_space.x + position.x) / _tile_width;
	const float z = (y * world_space.z + position.z) / _tile_length;

	_xf = x;
	_zf = z;

	select((int)x, (int)z);

	return false;
}

glm::vec3 TileSelection::get_select_position(glm::vec3 world_space, glm::vec3 offset) {
	float height = offset.y;
	const float increment = height > 1.0f ? height * 0.01f : 0.01f;

	while (above_terrain(world_space, offset, height)) {
		height -= increment;
	}

	const float y = abs((offset.y - height) / world_space.y);
	const float x = (y * world_space.x + offset.x) / _tile_width;
	const float z = (y * world_space.z + offset.z) / _tile_length;

	return glm::vec3(x, y, z);
}

bool TileSelection::above_terrain(glm::vec3 world_space, glm::vec3 position, float height) {
	const float y = abs((position.y - height) / world_space.y);
	const float x = (y * world_space.x + position.x) / _tile_width;
	const float z = (y * world_space.z + position.z) / _tile_length;

	const float terrain_height = exact_height(x, z);

	if(height > terrain_height) {
		return true;
	}

	return false;
}

float TileSelection::exact_height(float x, float z) {
	if(x < 0.0f || x >= _width || z < 0.0f || z >= _length) {
		return 0.0f;
	}

	const int index = (int)z * _width + (int)x;
	if(_height_map[index].is_flat()) {
		return _height_map[index].height[0];
	}

	const float width_y = _height_map[index].height[1] - _height_map[index].height[0];
	const float length_y = _height_map[index].height[2] - _height_map[index].height[0];
	const float min_height = _height_map[index].min_height();

	const bool left_to_right = width_y > 0.0f;
	const bool bottom_to_top = length_y > 0.0f;

	float height_x = 0.0f;
	float height_z = 0.0f;
	if(!left_to_right) {
		height_x = (1.0f - (float(x - int(x)))) * abs(width_y);
	}
	else {
		height_x = (float(x - int(x))) * abs(width_y);
	}

	if(!bottom_to_top) {
		height_z = (1.0f - (float(z - int(z)))) * abs(length_y);
	}
	else {
		height_z = (float(z - int(z))) * abs(length_y);
	}

	if(width_y != 0) {
		height_x += min_height;
	}
	if(length_y != 0) {
		height_z += min_height;
	};

	return height_x + height_z;;
}

glm::vec2 TileSelection::get_selected_tile() {
	return glm::vec2(_x, _z);
}


/********************************************************************************************************************************************************/

TerrainEntityPlacement::TerrainEntityPlacement() :
	_entity_index			( -1 ),
	_entity_position		( -1 ),
	_entity_x_index			( -1 ),
	_entity_z_index			( -1 ),
	_valid_entity_index	    ( false )
{}

void TerrainEntityPlacement::select_entity_index() {
	int x_index = 0;
	int z_index = 0;

	int x_position = 0;
	int z_position = 0;

	float decimal = _xf - _x;
	if (decimal < .25f) {
		x_index = _x * 2;
		x_position = LEFT;
	}
	else if (decimal >= .25f && decimal < .75f) {
		x_index = 1 + _x * 2;
		x_position = CENTER;
	}
	else if (decimal >= .75f && decimal < 1.0f) {
		x_index = (_x + 1) * 2;
		x_position = RIGHT;
	}

	decimal = _zf - _z;
	if (decimal < .25f) {
		z_index = _z * 2;
		z_position = TOP;
	}
	else if (decimal >= .25f && decimal < .75f) {
		z_index = 1 + _z * 2;
		z_position = CENTER;
	}
	else if (decimal >= .75f && decimal < 1.0f) {
		z_index = (_z + 1) * 2;
		z_position = BOTTOM;
	}

	_entity_x_index = x_index;
	_entity_z_index = z_index;
	_entity_index = z_index * _width + x_index;

	entity_position(x_position, z_position);
	entity_tile_height(_index, _entity_position, &_valid_entity_placement);
}

void TerrainEntityPlacement::entity_position(int x_position, int z_position) {
	if (x_position == CENTER) {
		_entity_position = z_position;
	}
	else if (z_position == CENTER) {
		_entity_position = x_position;
	}
	else if (x_position == LEFT && z_position == TOP) {
		_entity_position = TOP_LEFT;
	}
	else if (x_position == LEFT && z_position == BOTTOM) {
		_entity_position = BOTTOM_LEFT;
	}
	else if (x_position == RIGHT && z_position == TOP) {
		_entity_position = TOP_RIGHT;
	}
	else if (x_position == RIGHT && z_position == BOTTOM) {
		_entity_position = BOTTOM_RIGHT;
	}
}

bool TerrainEntityPlacement::is_valid_ramp_placement(int position, const GLfloat* center, const GLfloat* horizontal, const GLfloat* vertical, const GLfloat* diagonal) {
	if(!center || !horizontal || !vertical || !diagonal || position < TOP_LEFT) {
		return false;
	}
	// indexs
	int ci = 0, vi = 0, hi = 0, di = 0;

	switch(position) {
	case TOP_RIGHT :		ci = 1;		hi = 0;		vi = 3; 	di = 2;		break;
	case TOP_LEFT :			ci = 0;		hi = 1;		vi = 2;		di = 3;		break;
	case BOTTOM_RIGHT :		ci = 3;		hi = 2;		vi = 1;		di = 0;		break;
	case BOTTOM_LEFT :		ci = 2;		hi = 3;		vi = 0;		di = 1;		break;
	default:																break;
	}
	
	const float h1 = center[ci], h2 = vertical[vi], h3 = horizontal[hi], h4 = diagonal[di];

	return (h1 == h2 && h1 == h3 && h1 == h4 && h2 == h3 && h2 == h4 && h3 == h4);
}

float TerrainEntityPlacement::entity_tile_height(int index, int position, bool* valid_placement) {
	if (valid_placement) {
		*valid_placement = false;
	}

	if (!valid_index(index)) {
		return 0.0f;
	}

	float height = 0.0f;
	int i1 = index, i2 = index, i3 = index, i4 = index;
	
	switch (_entity_position) {
	case LEFT:		  i2 -= 1;															break;
	case RIGHT:		  i2 += 1;															break;
	case TOP:		  i2 -= _width;														break;
	case BOTTOM:	  i2 += _width;														break;
	case TOP_LEFT:	  i2 -= 1;		i3 -= _width;		i4 -= _width + 1;				break;
	case BOTTOM_LEFT: i2 -= 1;		i3 += _width;		i4 += _width - 1;				break;
	case TOP_RIGHT:	  i2 += 1;		i3 -= _width;		i4 -= _width - 1;				break;
	case BOTTOM_RIGHT:i2 += 1;		i3 += _width;		i4 += _width + 1;				break;
	default:																			break;
	}

	if (position == TOP || position == BOTTOM || position == LEFT || position == RIGHT) {
		if (valid_index(i2)) {
			const auto h1 = _height_map[i1].min_height();
			const auto h2 = _height_map[i2].min_height();

			height = max(h1, h2);

			const auto t1 = _height_map[i1].height;
			const auto t2 = _height_map[i2].height;

			switch (position) {
			case TOP:		_valid_entity_placement = (t1[0] == t1[1] && t1[0] == t2[2] && t1[1] == t2[3] && t2[2] == t2[3]);	break;
			case BOTTOM:	_valid_entity_placement = (t1[2] == t1[3] && t1[2] == t2[0] && t1[3] == t2[1] && t2[0] == t2[1]);	break;
			case LEFT:		_valid_entity_placement = (t1[0] == t1[2] && t1[0] == t2[1] && t1[2] == t2[3] && t2[1] == t2[3]);	break;
			case RIGHT:		_valid_entity_placement = (t1[1] == t1[3] && t1[1] == t2[0] && t1[3] == t2[2] && t2[0] == t2[2]);	break;
			default:																											break;
			}
		}
	}
	else {
		const auto h1 = _height_map[_index].min_height();
		float h2 = 0.0f, h3 = 0.0f, h4 = 0.0f;

		const bool f1 = _height_map[_index].is_flat();
		bool f2 = true, f3 = true, f4 = true;

		const GLfloat* t1 = _height_map[_index].height;
		GLfloat* t2 = nullptr; GLfloat* t3 = nullptr; GLfloat* t4 = nullptr;

		if (valid_index(i2)) {
			t2 = _height_map[i2].height;
			h2 = _height_map[i2].min_height();
			f2 = _height_map[i2].is_flat();
			height = max(h1, h2);
		}
		if (valid_index(i3)) {
			t3 = _height_map[i3].height;
			h3 = _height_map[i3].min_height();
			f3 = _height_map[i3].is_flat();
			height = max(height, h3);
		}
		if (valid_index(i4)) {
			t4 = _height_map[i4].height;
			h4 = _height_map[i4].min_height();
			f4 = _height_map[i4].is_flat();
			height = max(height, h4);
		}
		
		if (valid_placement) {
			// 4 tiles are flat and the same height
			if ((f1 & f2 & f3 & f4) && h1 == h2 && h1 == h3 && h1 == h4 && h2 == h3 && h2 == h4 && h3 == h4) {
				*valid_placement = true;
			}
			else {
				*valid_placement = is_valid_ramp_placement(position, t1, t2, t3, t4);
			}
		}

	}

	return height;
}

bool TerrainEntityPlacement::is_valid_entity_placement() {
	return _valid_entity_placement;
}

int TerrainEntityPlacement::entity_index(int x, int z, int position) {
	const int center = z * (_width * 2) + _width + 1 + x * 2;

	switch(position) {
	case CENTER :				return center;
	case TOP :					return center - _width;
	case BOTTOM :				return center + _width;
	case LEFT :					return center - 1;
	case RIGHT :				return center + 1;
	case TOP_LEFT: 				return center - _width - 1;
	case TOP_RIGHT :			return center - _width + 1;
	case BOTTOM_LEFT :			return center + _width - 1;
	case BOTTOM_RIGHT :			return center + _width + 1;
	}

	assert(0);
	return -1;
}

bool TerrainEntityPlacement::is_valid_entity_index(int index) {
	return (index >= 0 && index < _width * _length * 2);
}

/********************************************************************************************************************************************************/

TerrainEntities::TerrainEntities() {
	_entities.resize(_width * _length + ((_width - 1) * (_length - 1)));
}

void TerrainEntities::add_entity(std::shared_ptr<Entity> entity) {
	std::cout << _entity_index << '\n';
	_entities[_entity_index] = entity;

	const auto transform = entity->get<TransformComponent>();
	transform->_transform.set_position(entity_placement());
}

std::shared_ptr<Entity> TerrainEntities::remove_entity() {
	if(!_valid_entity_index) {
		return nullptr;
	}

	const auto entity = _entities[_entity_index];
	_entities[_entity_index] = nullptr;
	return entity;
}

std::shared_ptr<Entity> TerrainEntities::get_entity() {
	if(!_valid_entity_index) {
		return nullptr;
	}

	return _entities[_entity_index];
}

void TerrainEntities::adjust_entity_height() {
	if(!_valid_index) {
		return;
	}

	for (unsigned int i = 0; i < TOTAL_POSITIONS; ++i) {
		const int index = entity_index(_x, _z, i); 
		if (is_valid_entity_index(index)) {
			const auto entity = _entities[index];
			if (entity) {
				const auto transform = entity->get<TransformComponent>();
				const auto position = transform->_transform.get_position();
				float height = entity_tile_height(_index, i);
				transform->_transform.set_position(glm::vec3(position.x, height, position.z));
			}
		}
	}
}

bool TerrainEntities::is_empty_tile() {
	if(!_valid_index) {
		return false;
	}

	return _entities[_index] == nullptr ? true : false;
}

// broke needs to use _entity_index
std::shared_ptr<Entity> TerrainEntities::select_entity(glm::vec3 world_space, glm::vec3 position) {
	float height = position.y;
	const float increment = height > 1.0f ? height * 0.01f : 0.01f;

	while (above_entity(world_space, position, height)) {
		height -= increment;

		if(height <= 0.0f) {
			return nullptr;
		}
	}

	const float y = abs((position.y - height) / world_space.y);
	const float x = (y * world_space.x + position.x) / _tile_width;
	const float z = (y * world_space.z + position.z) / _tile_length;

	const int index = (int)z * _width + (int)x;

	std::cout << index << '\n';

	return _entities[index];
}

// broke needs to use _entity_index
bool TerrainEntities::above_entity(glm::vec3 world_space, glm::vec3 position, float height) {
	const float y = abs((position.y - height) / world_space.y);
	const float x = (y * world_space.x + position.x) / _tile_width;
	const float z = (y * world_space.z + position.z) / _tile_length;

	const float e_height = entity_height((int)x, (int)z);

	if (height > e_height) {
		return true;
	}

	return false;
}

// broke needs to use _entity_index
float TerrainEntities::entity_height(int x, int z) {
	if (x < 0 || x >= _width || z < 0 || z >= _length) {
		return 0.0f;
	}

	const int index = z * _width + x;

	if (_entities[index] != nullptr) {
		std::cout << "max y: " << _entities[index]->get<TransformComponent>()->_transform.get_position().y << '\n';
		const auto transform = _entities[index]->get<TransformComponent>();
		return transform->_collision_box.max.y + transform->_transform.get_position().y;
	}

	return 0.0f;
}

glm::vec3 TerrainEntities::entity_placement() {
	return glm::vec3(_entity_x_index / _tile_length / 2.0f, entity_tile_height(_index, _entity_position), _entity_z_index / _tile_length / 2.0f);
}

/********************************************************************************************************************************************************/

Terrain::Terrain(int width, int length, float tile_width, float tile_length) :
	TerrainData			( width, length, tile_width, tile_length )
{
	generate_vertex_data();
	generate_uv_data();
	generate_position_data();

	load_textures();

	create_vao();
}

Terrain::Terrain(TerrainData&& terrain_data) noexcept :
	TerrainData		( std::move(terrain_data) )
{
	generate_vertex_data();
	generate_uv_data();
	generate_position_data();

	load_textures();

	create_vao();
}

Terrain::~Terrain() {
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vertex_buffer);
	glDeleteBuffers(1, &_uv_buffer);

	glDeleteTextures(1, &_height_texture);
	glDeleteTextures(1, &_tile_texture._id);
}

void Terrain::generate_vertex_data() {
	_vertex_data.reserve(6 * 5);

	_vertex_data.push_back(glm::vec2(0.0f, 0.0f)); // height
	_vertex_data.push_back(glm::vec2(_tile_width, 0.0f)); // height
	_vertex_data.push_back(glm::vec2(0.0f, 0.0f));
	_vertex_data.push_back(glm::vec2(0.0f, 0.0f));
	_vertex_data.push_back(glm::vec2(_tile_width, 0.0f)); // height
	_vertex_data.push_back(glm::vec2(_tile_width, 0.0f));

	_vertex_data.push_back(glm::vec2(_tile_width, 0.0f)); // height
	_vertex_data.push_back(glm::vec2(_tile_width, _tile_length)); // height
	_vertex_data.push_back(glm::vec2(_tile_width, 0.0f));
	_vertex_data.push_back(glm::vec2(_tile_width, 0.0f));
	_vertex_data.push_back(glm::vec2(_tile_width, _tile_length)); // height
	_vertex_data.push_back(glm::vec2(_tile_width, _tile_length));

	_vertex_data.push_back(glm::vec2(_tile_width, _tile_length)); // height
	_vertex_data.push_back(glm::vec2(0.0f, _tile_length)); // height
	_vertex_data.push_back(glm::vec2(_tile_width, _tile_length));
	_vertex_data.push_back(glm::vec2(_tile_width, _tile_length));
	_vertex_data.push_back(glm::vec2(0.0f, _tile_length)); // height
	_vertex_data.push_back(glm::vec2(0.0f, _tile_length));

	_vertex_data.push_back(glm::vec2(0.0f, _tile_length)); // height
	_vertex_data.push_back(glm::vec2(0.0f, 0.0f)); // height
	_vertex_data.push_back(glm::vec2(0.0f, _tile_length));
	_vertex_data.push_back(glm::vec2(0.0f, _tile_length));
	_vertex_data.push_back(glm::vec2(0.0f, 0.0f)); // height
	_vertex_data.push_back(glm::vec2(0.0f, 0.0f));

	_vertex_data.push_back(glm::vec2(_tile_width, 0.0f)); // height
	_vertex_data.push_back(glm::vec2(0.0f, 0.0f));  // height
	_vertex_data.push_back(glm::vec2(0.0f, _tile_length)); // height
	_vertex_data.push_back(glm::vec2(_tile_width, 0.0f)); // height
	_vertex_data.push_back(glm::vec2(0.0f, _tile_length)); // height
	_vertex_data.push_back(glm::vec2(_tile_width, _tile_length)); // height
}

void Terrain::generate_uv_data() {
	_uv_data.reserve(6 * 5);
	
	_uv_data.push_back(glm::vec2(0, 0));
	_uv_data.push_back(glm::vec2(0.5, 0));
	_uv_data.push_back(glm::vec2(0, 1));
	_uv_data.push_back(glm::vec2(0, 1));
	_uv_data.push_back(glm::vec2(0.5, 0));
	_uv_data.push_back(glm::vec2(0.5, 1));

	_uv_data.push_back(glm::vec2(0, 0));
	_uv_data.push_back(glm::vec2(0.5, 0));
	_uv_data.push_back(glm::vec2(0, 1));
	_uv_data.push_back(glm::vec2(0, 1));
	_uv_data.push_back(glm::vec2(0.5, 0));
	_uv_data.push_back(glm::vec2(0.5, 1));

	_uv_data.push_back(glm::vec2(0, 0));
	_uv_data.push_back(glm::vec2(0.5, 0));
	_uv_data.push_back(glm::vec2(0, 1));
	_uv_data.push_back(glm::vec2(0, 1));
	_uv_data.push_back(glm::vec2(0.5, 0));
	_uv_data.push_back(glm::vec2(0.5, 1));

	_uv_data.push_back(glm::vec2(0, 0));
	_uv_data.push_back(glm::vec2(0.5, 0));
	_uv_data.push_back(glm::vec2(0, 1));
	_uv_data.push_back(glm::vec2(0, 1));
	_uv_data.push_back(glm::vec2(0.5, 0));
	_uv_data.push_back(glm::vec2(0.5, 1));

	_uv_data.push_back(glm::vec2(1, 1));
	_uv_data.push_back(glm::vec2(0.5, 1));
	_uv_data.push_back(glm::vec2(0.5, 0.5));
	_uv_data.push_back(glm::vec2(1, 1));
	_uv_data.push_back(glm::vec2(0.5, 0.5));
	_uv_data.push_back(glm::vec2(1, 0.5));
}

void Terrain::generate_position_data() {
	for (int i = 0; i < _length; ++i) {
		for (int j = 0; j < _width; ++j) {
			_position_data.push_back(glm::vec2(j * _tile_width, i * _tile_length));
		}
	}
}

void Terrain::load_textures() {
	_tile_texture._id = SOIL_load_OGL_texture("Data\\Terrain\\tile.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	if (_tile_texture._id == 0) {
		std::cout << "SOIL RESULT " << SOIL_last_result() << " Data\\tiles.png" << '\n';
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Terrain::adjust_tile_height(float height) {
	if (!_valid_index) {
		return;
	}

	adjust_vertex_height(_index, 0, height);
	adjust_vertex_height(_index, 1, height);
	adjust_vertex_height(_index, 2, height);
	adjust_vertex_height(_index, 3, height);

	update_vao();

	adjust_entity_height();
}

void Terrain::adjust_ramp_height() {
	if (!_valid_index) {
		return;
	}

	const int back_index = _index - _width;
	const int front_index = _index + _width;
	const int left_index = _index - 1;
	const int right_index = _index + 1;

	if (valid_index(front_index) &&
		_height_map[front_index].is_flat() &&
		_height_map[front_index].height[0] > _height_map[_index].height[0]) {
		adjust_back_ramp();
	}
	else if(valid_index(back_index) && 
		_height_map[back_index].is_flat() &&
		_height_map[back_index].height[0] > _height_map[_index].height[0]) {
		adjust_front_ramp();
	}
	else if (valid_index(left_index) && valid_left_index(_index) &&
		_height_map[left_index].is_flat() &&
		_height_map[left_index].height[0] > _height_map[_index].height[0]) {
		adjust_right_ramp();
	}
	else if (valid_index(right_index) && valid_right_index(_index) &&
		_height_map[right_index].is_flat() &&
		_height_map[right_index].height[0] > _height_map[_index].height[0]) {
		adjust_left_ramp();
	}

	update_vao();
}

void Terrain::adjust_front_ramp() {
	const int front_index = _index + _width;
	const int back_index = _index - _width;

	if(!valid_index(front_index) || !valid_index(back_index)) {
		return;
	}

	adjust_vertex_height(_index, 0, get_vertex_height(back_index, 2));
	adjust_vertex_height(_index, 1, get_vertex_height(back_index, 3));

	adjust_vertex_height(_index, 2, get_vertex_height(_index, 0) / 2);
	adjust_vertex_height(_index, 3, get_vertex_height(_index, 1) / 2);

	adjust_vertex_height(front_index, 1, get_vertex_height(_index, 0) / 2);
	adjust_vertex_height(front_index, 0, get_vertex_height(_index, 1) / 2);
}

void Terrain::adjust_back_ramp() {
	const int front_index = _index + _width;
	const int back_index = _index - _width;

	if (!valid_index(front_index) || !valid_index(back_index)) {
		return;
	}

	adjust_vertex_height(_index, 2, get_vertex_height(front_index, 0));
	adjust_vertex_height(_index, 3, get_vertex_height(front_index, 1));

	adjust_vertex_height(_index, 0, get_vertex_height(_index, 2) / 2);
	adjust_vertex_height(_index, 1, get_vertex_height(_index, 3) / 2);

	adjust_vertex_height(back_index, 2, get_vertex_height(_index, 2) / 2);
	adjust_vertex_height(back_index, 3, get_vertex_height(_index, 3) / 2);
}

void Terrain::adjust_right_ramp() {
	const int left_index = _index - 1;
	const int right_index = _index + 1;

	if (!valid_index(left_index) || !valid_index(right_index)) {
		return;
	}

	adjust_vertex_height(_index, 0, get_vertex_height(left_index, 1));
	adjust_vertex_height(_index, 2, get_vertex_height(left_index, 3));

	adjust_vertex_height(_index, 1, get_vertex_height(_index, 0) / 2);
	adjust_vertex_height(_index, 3, get_vertex_height(_index, 2) / 2);

	adjust_vertex_height(right_index, 0, get_vertex_height(_index, 0) / 2);
	adjust_vertex_height(right_index, 2, get_vertex_height(_index, 2) / 2);
}

void Terrain::adjust_left_ramp() {
	const int left_index = _index - 1;
	const int right_index = _index + 1;

	if (!valid_index(left_index) || !valid_index(right_index)) {
		return;
	}

	adjust_vertex_height(_index, 1, get_vertex_height(right_index, 0));
	adjust_vertex_height(_index, 3, get_vertex_height(right_index, 2));

	adjust_vertex_height(_index, 0, get_vertex_height(_index, 1) / 2);
	adjust_vertex_height(_index, 2, get_vertex_height(_index, 3) / 2);

	adjust_vertex_height(left_index, 1, get_vertex_height(_index, 1) / 2);
	adjust_vertex_height(left_index, 3, get_vertex_height(_index, 3) / 2);
}

void Terrain::adjust_vertex_height(int index, int vertex, float height) {
	if(index < 0 || index >= (int)_height_map.size() ||
	   vertex < 0 || vertex > 3) {
		return;
	}

	_height_map[index].height[vertex] = height;

	glBindBuffer(GL_ARRAY_BUFFER, _height_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * index + sizeof(GLfloat) * vertex, sizeof(GLfloat), &_height_map[index].height[vertex]);
}

void Terrain::create_vao() {
	glCreateVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	_program = Environment::get().get_resource_manager()->get_program(TERRAIN_SHADER_ID)->_id;
	glUseProgram(_program);

	glCreateBuffers(1, &_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
	glNamedBufferStorage(_vertex_buffer, sizeof(glm::vec2) * _vertex_data.size(), &_vertex_data[0], 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glCreateBuffers(1, &_uv_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _uv_buffer);
	glNamedBufferStorage(_uv_buffer, sizeof(glm::vec2) * _uv_data.size(), &_uv_data[0], 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glCreateBuffers(1, &_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _position_buffer);
	glNamedBufferStorage(_position_buffer, sizeof(glm::vec2) * _position_data.size(), &_position_data[0], 0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribDivisor(2, 1);

	glCreateBuffers(1, &_height_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _height_buffer);
	glNamedBufferStorage(_height_buffer, sizeof(GLfloat) * 4 * _height_map.size(), &_height_map[0], GL_DYNAMIC_STORAGE_BIT);

	glCreateTextures(GL_TEXTURE_BUFFER, 1, &_height_texture);
	glTextureBuffer(_height_texture, GL_R32F, _height_buffer);

	glUniform1i(glGetUniformLocation(_program, "height"), 0);
	glUniform1i(glGetUniformLocation(_program, "tile_texture"), 1);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _tile_texture._id);
}

void Terrain::draw(int mode, bool draw_tile) {
	glBindVertexArray(_vao);
	glUseProgram(_program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, _height_texture);
	glBindTextureUnit(GL_TEXTURE0, _height_buffer);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _tile_texture._id);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glDrawArraysInstanced(mode, 0, 6 * 5, _position_data.size());

	if (draw_tile) {
		TileSelection::draw();
	}

	glDisable(GL_CULL_FACE);
}

float Terrain::get_tile_width() {
	return _tile_width;
}

float Terrain::get_tile_length() {
	return _tile_length;
}

TileHeight Terrain::get_tile_height(int x, int z) {
	if(z < 0 || z >= _length ||
		x < 0 || x >= _width) {
		return { 0, 0, 0, 0 };
	}

	const int index = z * _width + x;
	TileHeight tile;
	tile.height[0] = _height_map[index].height[0];
	tile.height[0] = _height_map[index].height[1];
	tile.height[0] = _height_map[index].height[2];
	tile.height[0] = _height_map[index].height[3];

	return tile;
}

float Terrain::get_vertex_height(int index, int vertex) {
	if(index < 0 || index > (int)_height_map.size() ||
	   vertex < 0 || vertex > 3) {
		return 0.0f;
	}

	return _height_map[index].height[vertex];
}

/********************************************************************************************************************************************************/