#pragma once
#include "Scene.h"
#include "Text.h"
//freetype (fonts) specific
#include <ft2build.h>
#include FT_FREETYPE_H

class Test_Scene : public Scene {
protected:
	virtual void init(int window_width, int window_height, const char* title) override;
public:
	TextRenderer* tr;

	Test_Scene();
	virtual void render(GLfloat deltaTime)override;
	virtual void update(GLfloat deltaTime, EventFeedback* feedback = nullptr)override;
	~Test_Scene();

	std::map<GLchar, Character> Characters;
	GLuint VAO, VBO;
};

