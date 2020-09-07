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
	Texture();
	Texture(int key, const char* file_path);
	~Texture();

	int _key;
	GLuint _id;
	std::string _type;
private:
	void load_from_file(const char* file_path);
};

#endif