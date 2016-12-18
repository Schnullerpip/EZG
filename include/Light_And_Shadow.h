#pragma once
#include "Scene.h"
#include "Input_Handler.h"
#include "Text.h"

class Light_And_Shadow : public Scene
{
protected:
	virtual void init(int window_width, int window_height, const char* title = "")override;
	Input_Handler* input;
	Shape* test;
	TextRenderer* tr;

	int number_samples;
public:
	Light_And_Shadow(Input_Handler* i, int number_samples);
	virtual void render()override;
	virtual void update(GLfloat deltaTime, EventFeedback* feedback = nullptr)override;
	~Light_And_Shadow();
};

