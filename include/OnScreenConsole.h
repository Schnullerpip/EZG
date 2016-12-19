#pragma once
#include "OnScreenMessage.h"
#include <deque>
#include <Text.h>
#include "Input_Handler.h"

class OnScreenConsole : public InputListener
{
	std::deque<OnScreenMessage*> messages;
	//killing interval
	GLfloat ki, count = 0;
	float WINDOW_WIDTH, WINDOW_HEIGHT;	
	TextRenderer* textRenderer;
	Input_Handler* input;
public:
	void out(OnScreenMessage* msg);
	void update(float deltatime);
	std::string in();
	OnScreenConsole(float ki, Input_Handler* input, int window_width, int window_height);
	OnScreenConsole();
	~OnScreenConsole();

	//InputListener
	virtual void actOnChange(eventType et)override;
};