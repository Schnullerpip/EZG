#pragma once
#include "OnScreenMessage.h"
#include <deque>
#include <Text.h>

class OnScreenConsole
{
	std::deque<OnScreenMessage*> messages;
	//killing interval
	GLfloat ki, count = 0;
	float WINDOW_WIDTH, WINDOW_HEIGHT;	
	TextRenderer* textRenderer;
public:
	void add(OnScreenMessage* msg);
	void update(float deltatime);
	OnScreenConsole(float ki, int window_width, int window_height);
	OnScreenConsole();
	~OnScreenConsole();
};

