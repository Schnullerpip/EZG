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
	std::vector<std::tuple<std::string, std::string*, const char*>> functions;
public:
	void out(OnScreenMessage* msg = nullptr);
	void out(std::string message);

	void registerCommand(bool*, const char*, const char* out_message = nullptr);
	int registerFunction(std::string command, std::string* argptr, const char* out_message = nullptr);
	std::string argsAt(int i)const;

	void update(float deltatime);
	void in();
	OnScreenConsole(float ki, Input_Handler* input, EventFeedback* fb, int window_width, int window_height);
	OnScreenConsole();
	~OnScreenConsole();

	//InputListener
	virtual void actOnChange(eventType et)override;
	bool isInInsertMode() const;
};
