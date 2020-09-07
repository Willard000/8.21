#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

typedef unsigned int GLuint;

struct ReadTextureFile {
	ReadTextureFile(const char* file_path);

	int			_icon_id = -1;
	std::string _type;
	std::string _texture;
};

struct Texture {
	Texture() :
		_key		( -1 ),
		_id			( 0 ),
		_type		( "" )
	{}

	Texture(int key, const char* file_path) :
		_key		( key )
	{
		load_from_file(file_path);
	}

	int _key;
	GLuint _id;
	std::string _type;
private:
	void load_from_file(const char* file_path);
};

#endif