#pragma once
#include <gl/glew.h>
#include <vector>
#include <SOIL.h>
class Texture
{
	std::vector<GLfloat> texCoords = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.5f, 1.0f
	};
	int width, height;
	unsigned char* image;
	GLuint texture = 0;
public:
	Texture(const char* path);
	Texture();
	~Texture();
	void use() const;
	void drop() const;
};

