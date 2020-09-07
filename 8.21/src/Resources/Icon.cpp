#include "Icon.h"

#include <GL/gl3w.h>

#include "../src/System/Environment.h"
#include "../src/System/ResourceManager.h"
#include "../src/Resources/Program.h"
#include "../src/Resources/GUI.h"
#include "../src/Utility/FileReader.h"
#include "../src/Resources/Texture.h"

#define ICON_SHADER 4

constexpr GLfloat vertex_data[12] = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f};

GUIDrawIcon::GUIDrawIcon() {
	create_vao();
}

GUIDrawIcon::~GUIDrawIcon() {
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vertex_buffer);
}

void GUIDrawIcon::create_vao() {
	glCreateVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	_program = Environment::get().get_resource_manager()->get_program(ICON_SHADER)->_id;
	glUseProgram(_program);

	glUniformMatrix4fv(glGetUniformLocation(_program, "projection"), 1, GL_FALSE, &GUI_PROJECTION[0][0]);

	glCreateBuffers(1, &_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
	glNamedBufferStorage(_vertex_buffer, sizeof(GLfloat) * 12, vertex_data, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void GUIDrawIcon::draw(GUIIconDesc icon_desc) {
	glBindVertexArray(_vao);
	glUseProgram(_program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, icon_desc._texture);

	glUniform1f(glGetUniformLocation(_program, "width"), icon_desc._width);
	glUniform1f(glGetUniformLocation(_program, "height"), icon_desc._height);
	glUniform2fv(glGetUniformLocation(_program, "position"), 1, &icon_desc._position[0]);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

ReadIconFile::ReadIconFile(const char* file_path) {
	FileReader file(file_path);
	file.set_section("Icon");
	file.read(&_id, "id");
	file.read(&_type, "type");
}

GUIIcon::GUIIcon() :
	_key			( -1 ),
	_id				( -1 )
{}

GUIIcon::GUIIcon(int id, std::string type, std::shared_ptr<Texture> texture) :
	_key			( -1 ),
	_id				( id ),
	_type			( type ),
	_texture		( texture )
{}

GUIIcon::GUIIcon(int key, const char* file_path) :
	_key			( key )
{
	load_from_file(file_path);
}

void GUIIcon::load_from_file(const char* file_path) {
	ReadIconFile icon_file(file_path);

	_id = icon_file._id;
	_type = icon_file._type;
	_texture = std::make_shared<Texture>(-1, file_path);
}