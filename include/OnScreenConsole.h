#pragma once
#include "OnScreenMessage.h"
#include <deque>
#include <Text.h>
#include "Input_Handler.h"
#include "EventFeedback.h"

class OnScreenConsole : public InputListener
{
	std::deque<OnScreenMessage*> messages;
	//killing interval
	GLfloat ki, count = 0;
	float WINDOW_WIDTH, WINDOW_HEIGHT;	
	TextRenderer* textRenderer;
	Input_Handler* input;
	EventFeedback* feedback;

	OnScreenMessage* current_input = nullptr;
	std::string last_command = "";
	bool insert_mode = false;
	void interpreteInput(std::string s);
	std::string clear();
public:
	void out(OnScreenMessage* msg = nullptr);
	void update(float deltatime);
	void in();
	OnScreenConsole(float ki, Input_Handler* input, EventFeedback* fb, int window_width, int window_height);
	OnScreenConsole();
	~OnScreenConsole();

	//InputListener
	virtual void actOnChange(eventType et)override;
	bool isInInsertMode() const;
};
