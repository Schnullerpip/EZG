#include "Input_Handler.h"
#include <iostream>
#include <GL/glew.h>

void Input_Handler::reset()
{
	first_mouse = true;
	for (int i = 0; i < 1024; ++i)
	{
		keys[i] = false;
	}
}

bool Input_Handler::is_pressed(int key, bool single_event)
{
	bool retval = ((key >= 1024) ? false : keys[key]);
	if (single_event)
		keys[key] = false;
	return retval;
}

void Input_Handler::press_at(int key) {
	if (key < 1024)
	{
		keys[key] = true;
	}
}

void Input_Handler::release_at(int key) {
	if (key < 1024)
	{
		keys[key] = false;
	}
}

void Input_Handler::mouse_move(double x, double y) {
	xpos = x;
	ypos = y;
	if (first_mouse)
	{
		lastx = xpos;
		lasty = ypos;
		first_mouse = false;
	}

	xoffset = xpos - lastx;
    yoffset = lasty - ypos; // Reversed since y-coordinates go from bottom to left
    lastx = xpos;
    lasty = ypos;

    GLfloat sensitivity = 0.05;	// Change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;
}

void Input_Handler::mouse_scroll(double x, double y) {
	scroll_count += (x + y);
}

Input_Handler::~Input_Handler()
{
}
