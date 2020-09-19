#ifndef MESH_H
#define MESH_H

#include <GL/gl3w.h>

#include <vector>

#include "Transform.h"
#include "Texture.h"

class Mesh {
public:
	Mesh();
	Mesh(
		const std::vector<Texture>& textures,
		const std::vector<glm::vec3>& vertices,
		const std::vector<glm::vec2>& uvs,
		const std::vector<glm::vec3>& normals,
		const std::vector<unsigned short>& indices
	);
	Mesh(const Mesh& rhs);
	Mesh(Mesh&& rhs) noexcept;
	~Mesh();

	void load_buffers();
	void update_buffers();
	void draw(const GLuint program, Transform& transform, int mode = GL_TRIANGLES);

	void delete_vao();
public:
	std::vector<Texture>        _textures;
	std::vector<glm::vec3>      _vertices;
	std::vector<glm::vec2>      _uvs;
	std::vector<glm::vec3>      _normals;
	std::vector<unsigned short> _indices;
private:
	void prepare_draw(const GLuint program, Transform& transform);
private:
	GLuint  _vao,
			_vertex_buffer,
			_uv_buffer,
			_normal_buffer,
			_indices_buffer;
};

#endif