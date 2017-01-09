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
	std::string last_command = "$>";
	bool insert_mode = false, versatile = true;
	void interpreteInput(std::string s);
	void proposal(std::string s) const;
	std::string clear();

	std::vector<std::tuple<bool*, const char*, const char*>> references;
public:
	void out(OnScreenMessage* msg = nullptr);
	void out(std::string message);

	void registerCommand(bool*, const char*, const char* out_message = nullptr);

	void update(float deltatime);
	void in();
	OnScreenConsole(float ki, Input_Handler* input, EventFeedback* fb, int window_width, int window_height);
	OnScreenConsole();
	~OnScreenConsole();

	//InputListener
	virtual void actOnChange(eventType et)override;
	bool isInInsertMode() const;
};
