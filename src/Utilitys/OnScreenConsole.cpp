#include "OnScreenConsole.h"


void OnScreenConsole::add(OnScreenMessage* msg)
{
	messages.push_back(msg);

	//if the screen is packed -> pop_front
	float accumulated_height = 0;
	for (auto msg : messages)
	{
	}
	for (int i = 0; i < messages.size(); ++i)
	{
		OnScreenMessage* msg = messages[i];
		accumulated_height += msg->scale * textRenderer->getPixelHeight()*i;
	}
	if (accumulated_height >= WINDOW_HEIGHT)
	{
		messages.pop_front();
	}
}

void OnScreenConsole::update(float deltatime)
{
	if ((count += deltatime) >= ki && !messages.empty())
	{
		OnScreenMessage* msg = messages.front();
		messages.pop_front();
		delete msg;
		count = 0;
	}

	int pH = textRenderer->getPixelHeight();
	for (int i = 0; i < messages.size(); ++i)
	{
		OnScreenMessage* msg = messages[i];
		textRenderer->render(msg->message, 0.f, WINDOW_HEIGHT - pH*i - pH*msg->scale, msg->scale, msg->color, WINDOW_WIDTH, WINDOW_HEIGHT);
	}
}

OnScreenConsole::OnScreenConsole(float ki, int window_width, int window_height):OnScreenConsole()
{
	this->ki = ki;
	WINDOW_WIDTH = window_width;
	WINDOW_HEIGHT = window_height;
}

OnScreenConsole::OnScreenConsole()
{
	TextRenderer::Init();
	textRenderer = new TextRenderer();
}

OnScreenConsole::~OnScreenConsole()
{
	for (auto msg : messages)
	{
		delete msg;
	}
}