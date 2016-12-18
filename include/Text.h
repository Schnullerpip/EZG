#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <map>
#include <Shader.h>

//Holds all the state information relevant to a character loaded by freetype
struct Character
{
	GLuint TextureID; //ID handle
	glm::ivec2 Size;  //Size of glyph
	glm::ivec2 Bearing;//Offset from baseline
	GLuint Advance;//Horizontal offset to advance to next glyph
};


class TextRenderer
{
	GLuint VBO, VAO;
	Shader* default_text_shader; 
	int pixel_height;

public:
	static void Init();
	void render(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, int WIDTH, int HEIGHT) const;
	int getPixelHeight()const;

	TextRenderer(int pixelheight = 48);
	~TextRenderer();
	
};

