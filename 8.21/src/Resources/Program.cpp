#include "Program.h"

#include <vector>
#include <fstream>
#include <string>
#include <sstream>

#include <iostream>

#include "../src/Utility/FileReader.h"

ReadProgramFile::ReadProgramFile(const char* file_path) {
	FileReader file(file_path);
	file.set_section("Program");
	file.read(&_dir, "DIR");
	file.read(&_name, "name");
	file.read(&_vertex_path, "vertex");
	file.read(&_fragment_path, "fragment");
	file.read(&_geometry_path, "geometry");
	file.read(&_compute_path, "compute");
}

void Program::load_from_file(const char* file_path) {
	ReadProgramFile program_file(file_path);
	ShaderInfo program[] = { {GL_VERTEX_SHADER, }, {GL_GEOMETRY_SHADER, }, { GL_FRAGMENT_SHADER, }, {GL_COMPUTE_SHADER, }, { GL_NONE, } };
	//	Program program[] = { {GL_VERTEX_SHADER, }, { GL_FRAGMENT_SHADER, }, { GL_NONE, } };

	auto append_strings = [](std::string& str, const std::string_view str2, const std::string_view str3) {
		str.reserve(str2.size() + str3.size());
		str.append(str2);
		str.append(str3);
	};

	if (!program_file._vertex_path.empty())		append_strings(program[0]._file_path, program_file._dir, program_file._vertex_path);
	if (!program_file._geometry_path.empty())	append_strings(program[1]._file_path, program_file._dir, program_file._geometry_path);
	if (!program_file._fragment_path.empty())	append_strings(program[2]._file_path, program_file._dir, program_file._fragment_path);
	if (!program_file._compute_path.empty())	append_strings(program[3]._file_path, program_file._dir, program_file._compute_path);

	_id = load_shaders(program);
	_name = program_file._name;
}

GLuint load_shaders(const ShaderInfo* program) {
	const GLuint program_id = glCreateProgram();
	std::vector<GLuint> shader_ids;
	const char* data;
	GLint result;
	GLint info_log_length;
	bool loaded = true;
	GLuint shader_id = 0;

	for (unsigned int i = 0; program[i]._type != GL_NONE; ++i) {
		std::string sdata;
		std::stringstream ssdata;
		std::fstream shader_file(program[i]._file_path, std::ios::in);
		if (shader_file.is_open()) {
			ssdata << shader_file.rdbuf();
			shader_file.seekg(0, std::ios::end);
			sdata.reserve((size_t)shader_file.tellg());
			sdata = ssdata.str();
			data = sdata.c_str();

			shader_id = glCreateShader(program[i]._type);
			shader_ids.push_back(shader_id);
			loaded = true;
		}
		else {
			loaded = false;
		}

		if (loaded) {
			glShaderSource(shader_id, 1, &data, NULL);
			glCompileShader(shader_id);

			glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
			std::cout << "Shader Result -- " << program[i]._file_path << " -- " << ((result == GL_TRUE) ? "Good" : "Bad") << '\n';
			glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
			if (info_log_length != 0) {
				GLchar* info_log = new GLchar[info_log_length];
				glGetShaderInfoLog(shader_id, info_log_length, 0, info_log);
				std::cout << "-- Shader Log --" << std::endl << info_log << '\n';
				std::cout << "-- " << program[i]._file_path << "-- " << std::endl << data << '\n';
				delete[] info_log;
			}
		}
	}

	for (auto shader_id : shader_ids) {
		glAttachShader(program_id, shader_id);
	}

	glLinkProgram(program_id);

	for (auto shader_id : shader_ids) {
		glDetachShader(program_id, shader_id);
		glDeleteShader(shader_id);
	}

	std::cout << "Program id: " << program_id << std::endl;

	return program_id;
}