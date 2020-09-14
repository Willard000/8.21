#ifndef MODEL_H
#define MODEL_H

#include <string_view>
#include <memory>
#include <vector>

#include "../src/Resources/Mesh.h"

struct Program;
class Transform;

typedef unsigned int GLuint;

struct ReadModelFile {
public:
	ReadModelFile(const char* file_path);

	std::string _directory;
	std::string _file;
	GLuint _program_key = 0;
};

class Model {
public:
	Model();
	Model(std::shared_ptr<Program> program, std::string_view directory, std::string_view model_file);
	Model(int id, std::string_view file_path);
	Model(const Model& rhs);
	~Model();

	void draw(Transform& transform);

	std::shared_ptr<Program> get_program();
private:
	bool load_from_file(const char* file_path);
	bool load_assimp(std::string_view directory, std::string_view path);
private:
	int _id;
	std::shared_ptr<Program> _program;
	std::vector<Mesh> _meshes;
};

#endif