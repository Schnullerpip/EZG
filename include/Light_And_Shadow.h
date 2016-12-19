#pragma once
#include "Scene.h"
#include "Input_Handler.h"
#include "OnScreenConsole.h"

class Light_And_Shadow : public Scene
{
protected:
	virtual void init(int window_width, int window_height, const char* title = "")override;
	Input_Handler* input;
	Shape* test;
	OnScreenConsole* console;
public:
	Light_And_Shadow(Input_Handler* i, EventFeedback* fb);
	virtual void render(GLfloat deltaTime)override;
	virtual void update(GLfloat deltaTime, EventFeedback* feedback = nullptr)override;
	~Light_And_Shadow();
};

