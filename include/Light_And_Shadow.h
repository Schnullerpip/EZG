#pragma once
#include "Scene.h"
#include "Input_Handler.h"

class Light_And_Shadow : public Scene
{
protected:
	virtual void init(int window_width, int window_height, const char* title = "")override;
	Input_Handler* input;
	Shape* test;
public:
	Light_And_Shadow(Input_Handler* i);
	virtual void render()override;
	virtual void update(GLfloat deltaTime)override;
	~Light_And_Shadow();
};

