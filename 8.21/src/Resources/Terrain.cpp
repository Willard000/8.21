#include "Terrain.h"

#include <SOIL/SOIL2.h>

#include "../src/System/Environment.h"
#include "../src/System/ResourceManager.h"
#include "../src/Resources/Program.h"

#include <iostream>

#define TERRAIN_SHADER_ID 1

Terrain::Terrain(int width, int length, float tile_width, float tile_length) :
	Tile_Selection			( width, length, tile_width, tile_length )
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
	if(index < 0 || index >= _height_map.size() ||
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

	Tile_Selection::draw();

	glDisable(GL_CULL_FACE);
}

float Terrain::get_tile_width() {
	return _tile_width;
}

float Terrain::get_tile_length() {
	return _tile_length;
}

Tile_Height Terrain::get_tile_height(int x, int z) {
	if(z < 0 || z >= _length ||
		x < 0 || x >= _width) {
		return { 0, 0, 0, 0 };
	}

	const int index = z * _width + x;
	Tile_Height tile;
	tile.height[0] = _height_map[index].height[0];
	tile.height[0] = _height_map[index].height[1];
	tile.height[0] = _height_map[index].height[2];
	tile.height[0] = _height_map[index].height[3];

	return tile;
}

float Terrain::get_vertex_height(int index, int vertex) {
	if(index < 0 || index > _height_map.size() ||
	   vertex < 0 || vertex > 3) {
		return 0.0f;
	}

	return _height_map[index].height[vertex];
}

Tile_Selection::Tile_Selection(int width, int height, float tile_width, float tile_length) :
	Terrain_Data (width, height, tile_width, tile_length)
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

void Tile_Selection::create_vao() {
	glCreateVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	_program = Environment::get().get_resource_manager()->get_program(0)->_id;
	glUseProgram(_program);

	glCreateBuffers(1, &_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
	glNamedBufferStorage(_vertex_buffer, sizeof(glm::vec3) * _vertex_data.size(), &_vertex_data[0], GL_DYNAMIC_STORAGE_BIT);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glUniformMatrix4fv(glGetUniformLocation(_program, "model"), 1, GL_FALSE, &_transform.get_model()[0][0]);
}

void Tile_Selection::update_vao() {
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

void Tile_Selection::select(int x, int z) {
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

void Tile_Selection::draw() {
	glBindVertexArray(_vao);
	glUseProgram(_program);

	glUniformMatrix4fv(glGetUniformLocation(_program, "model"), 1, GL_FALSE, &_transform.get_model()[0][0]);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_TRIANGLES, 0, _vertex_data.size());

	glDisable(GL_BLEND);
}

bool Tile_Selection::valid_index(int index) {
	return (index >= 0 && index < _width * _length);
}

bool Tile_Selection::valid_left_index(int index) {
	return (index / _width == (index + 1) / _width);
}

bool Tile_Selection::valid_right_index(int index) {
	return (index / _width == (index - 1) / _width);
}

Terrain_Data::Terrain_Data(int width, int length, float tile_width, float tile_length) :
	_width			 ( width ),
	_length			 ( length ),
	_tile_width		 ( tile_width ),
	_tile_length	 ( tile_length )
{
	_height_map.resize(width * length);
}