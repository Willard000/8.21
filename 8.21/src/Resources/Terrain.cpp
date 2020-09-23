#include "Terrain.h"

#include <SOIL/SOIL2.h>

#include "../src/System/Environment.h"
#include "../src/System/ResourceManager.h"
#include "../src/Resources/Program.h"

#include <iostream>

#define TERRAIN_SHADER_ID 1

/********************************************************************************************************************************************************/

TerrainData::TerrainData(int width, int length, float tile_width, float tile_length) :
	_width(width),
	_length(length),
	_tile_width(tile_width),
	_tile_length(tile_length)
{
	_height_map.resize(width * length);
}

TerrainData::TerrainData() :
	_width(0),
	_length(0),
	_tile_width(0),
	_tile_length(0)
{}

/********************************************************************************************************************************************************/

TileSelection::TileSelection() :
	_x				 ( 0 ),
	_z				 ( 0 ),
	_index			 ( 0 ),
	_valid_index	 ( false )
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

void TileSelection::create_vao() {
	glCreateVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	_program = Environment::get().get_resource_manager()->get_program(0)->_id;
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_TRIANGLES, 0, _vertex_data.size());

	glDisable(GL_BLEND);
}

bool TileSelection::valid_index(int index) {
	return (index >= 0 && index < _width* _length);
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

bool TileSelection::test_ray_height(glm::vec3 world_space, glm::vec3 position) {

	float height = position.y;
	const float increment = height > 1.0f ? height * 0.01f : 0.01f;

	while (ray_above_terrain(world_space, position, height)) {
		height -= increment;
	}

	const float y = abs((position.y - height) / world_space.y);
	const float x = (y * world_space.x + position.x) / _tile_width;
	const float z = (y * world_space.z + position.z) / _tile_length;

	select((int)x, (int)z);

	return false;
}

bool TileSelection::ray_above_terrain(glm::vec3 world_space, glm::vec3 position, float height) {
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

/********************************************************************************************************************************************************/

TerrainEntities::TerrainEntities() {
	_entities.resize(_width * _length);
}

void TerrainEntities::add_entity(std::shared_ptr<Entity> entity) {
	if(!_valid_index) {
		return;
	}

	_entities[_index] = entity;

	const auto transform = entity->get<TransformComponent>();
	transform->_transform.set_position(glm::vec3(_x + _tile_width / 2, _height_map[_index].min_height(), _z + _tile_length / 2));
}

std::shared_ptr<Entity> TerrainEntities::remove_entity() {
	if(!_valid_index) {
		return nullptr;
	}

	const auto entity = _entities[_index];
	_entities[_index] = nullptr;
	return entity;
}

std::shared_ptr<Entity> TerrainEntities::get_entity() {
	if(!_valid_index) {
		return nullptr;
	}

	return _entities[_index];
}

void TerrainEntities::adjust_entity_height() {
	if(!_valid_index) {
		return;
	}

	const auto entity = _entities[_index];
	if(!entity) {
		return;
	}

	const auto transform = entity->get<TransformComponent>();
	transform->_transform.set_position(glm::vec3(_x + _tile_width / 2, _height_map[_index].min_height(), _z + _tile_length / 2));

}

bool TerrainEntities::is_empty_tile() {
	if(!_valid_index) {
		return false;
	}

	return _entities[_index] == nullptr ? true : false;
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

void Terrain::draw(int mode) {
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

	TileSelection::draw();

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