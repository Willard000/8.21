#include "Mesh.h"

Mesh::Mesh() {
	load_buffers();
}

Mesh::Mesh(
	const std::vector<Texture>& textures,
	const std::vector<glm::vec3>& vertices,
	const std::vector<glm::vec2>& uvs,
	const std::vector<glm::vec3>& normals,
	const std::vector<unsigned short>& indices
) :
	_textures			( textures ),
	_vertices			( vertices ),
	_uvs				( uvs ),
	_normals			( normals ),
	_indices			( indices )
{
	load_buffers();
}

Mesh::~Mesh() {
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vertex_buffer);
	glDeleteBuffers(1, &_uv_buffer);
	glDeleteBuffers(1, &_normal_buffer);
	glDeleteBuffers(1, &_indices_buffer);

	for(const auto texture : _textures) {
		glDeleteTextures(1, &texture._id);
	}
}

void Mesh::load_buffers() {
	glCreateVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glCreateBuffers(1, &_vertex_buffer);
	if (_vertices.size() > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
		glNamedBufferStorage(_vertex_buffer, sizeof(glm::vec3) * _vertices.size(), &_vertices[0], 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	glCreateBuffers(1, &_uv_buffer);
	if (_uvs.size() > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, _uv_buffer);
		glNamedBufferStorage(_uv_buffer, sizeof(glm::vec2) * _uvs.size(), &_uvs[0], 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	glCreateBuffers(1, &_normal_buffer);
	if (_normals.size() > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, _normal_buffer);
		glNamedBufferStorage(_normal_buffer, sizeof(glm::vec3) * _normals.size(), &_normals[0], 0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	glCreateBuffers(1, &_indices_buffer);
	if (_indices.size() > 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indices_buffer);
		glNamedBufferStorage(_indices_buffer, sizeof(unsigned short) * _indices.size(), &_indices[0], 0);
	}
}

void Mesh::draw(const GLuint program, Transform& transform, int mode) {
	glBindVertexArray(_vao);
	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &transform.get_model()[0][0]);

	for (unsigned int i = 0; i < _textures.size(); ++i) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, _textures[i]._id);
	}

	glDrawElements(mode, _indices.size(), GL_UNSIGNED_SHORT, (void*)0);
}