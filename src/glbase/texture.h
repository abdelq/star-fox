#pragma once
#include <main.h>

class Texture
{
public:
	Texture(const char* filepath);
	~Texture();

	unsigned int glID() { return _id; }
	unsigned int width() { return _width; }
	unsigned int height() { return _height; }
protected:
	unsigned int _id, _width, _height;
	unsigned short _depth;

	std::vector<char> _data;
};