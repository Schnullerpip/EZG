#pragma once
#include <GL/glew.h>
#include <vector>
#include "InputListener.h"

enum eventType {KEYPRESSED, KEYRELEASED, MOUSEMOVED, MOUSESCROLL /*, more to follow*/};

class Input_Handler
{
	//key relevant
	bool keys[1024];

	//eventmanagement
	std::vector<InputListener*> listeners;

public:
	//mouse relevant
	double xpos, ypos, lastx, lasty;
	GLfloat xoffset, yoffset;
	GLfloat scroll_count;
	bool first_mouse = true;

	//useful
	void reset();

	//key handling
	bool is_pressed(int key, bool single_event = true);
	void press_at(int key);
	void release_at(int key);
	char last_input;

	//mouse handling
	void mouse_move(double x, double y);
	void mouse_scroll(double x, double y);

	//event-anagement
	void subscribe(InputListener* listener);
	void triggerListeners(eventType et);


	Input_Handler(int window_width, int window_height): xpos(0), ypos(0)
	{
		lastx = window_width / 2;
		lasty = window_height / 2;
	}

	~Input_Handler();
};

