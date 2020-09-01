#ifndef FONT_MAP_H
#define FONT_MAP_H

#include <map>
#include <glm/gtc/matrix_transform.hpp>

struct Character {
	int codePoint, x, y, width, height, originX, originY;
};

struct Font {
	const char* name;
	int size, bold, italic, width, height, characterCount;
	const Character* characters;
};

class FontMap {
public:
	FontMap();

	const Character* get(char c);
	const float width(char c);
	const float height(char c);
	const glm::vec2 position(char c);
private:
	std::map<char, const Character*> _font_map;
};

#endif