#include "Texture.h"

#include "../src/Utility/FileReader.h"

#include <GLFW/glfw3.h>
#include <SOIL/SOIL2.h>
#include <iostream>


ReadTextureFile::ReadTextureFile(const char* file_path)
{
	FileReader file(file_path);
	file.set_section("Texture");
	file.read(&_type, "type");
	file.read(&_texture, "texture");
}

Texture::Texture() :
	_key			( -1 ),
	_id				( 0 ),
	_type			( "" )
{}

Texture::Texture(int key, const char* file_path) :
	_key			( key )
{
	load_from_file(file_path);
}

Texture::Texture(const Texture& rhs) :
	_key			 ( rhs._key ),
	_id				 ( rhs._id ),
	_type			 ( rhs._type )
{}	

void Texture::delete_texture() {
	glDeleteTextures(1, &_id);
}

void Texture::load_from_file(const char* file_path) {
	ReadTextureFile texture_file(file_path);

	_type = texture_file._type;

	_id = SOIL_load_OGL_texture(texture_file._texture.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	if (_id == 0) {
		std::cout << "SOIL RESULT " << SOIL_last_result() << " " << texture_file._texture << '\n';
	}

	std::cout << "texture id: " << _id << '\n';

	//texture paramenters...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}