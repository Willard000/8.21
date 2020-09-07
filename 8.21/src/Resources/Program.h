#ifndef PROGRAM_H
#define PROGRAM_H

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <string>

struct ShaderInfo {
	unsigned short _type;
	std::string _file_path;
};

// last address in program must end with type = GL_NONE
GLuint load_shaders(const ShaderInfo* program);

struct ReadProgramFile {
	ReadProgramFile(const char* file_path);

	std::string _dir;
	std::string _name;

	std::string _vertex_path;
	std::string _fragment_path;
	std::string _geometry_path;
	std::string _compute_path;
};

struct Program {
	Program();
	Program(int key, const char* file_path);
	~Program();

	int			 _key;
	std::string  _name;
	GLuint		 _id;
private:
	void load_from_file(const char* file_path);
};

#endif