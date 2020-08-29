#ifndef TEXTURE_H
#define TEXTURE_H

#include <GLFW/glfw3.h>
#include <string>

struct Texture {
	GLuint id = 0;
	std::string type;
};

#endif