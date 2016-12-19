#include "OnScreenConsole.h"
#include "Input_Handler.h"
#include <GLFW/glfw3.h>



std::string OnScreenConsole::clear()
{
	std::string retVal = current_input->message;
	while (messages.size() > 0)
	{
		OnScreenMessage* msg = messages.front();
		messages.pop_front();
		delete msg;
	}
	current_input = nullptr;
	count = 0;
	return retVal;
}

void OnScreenConsole::out(OnScreenMessage* msg)
{
	//if no msg was given - output the current assembled line
	if (!msg && !current_input)
	{
		messages.push_back(current_input);
	}
	else
	{
		messages.push_back(msg);
	}


	//if the screen is packed -> pop_front
	float accumulated_height = 0;
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

	if (!insert_mode && (count += deltatime) >= ki && !messages.empty())
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

void OnScreenConsole::in()
{
	out(new OnScreenMessage("$>"));
	insert_mode = true;
}

OnScreenConsole::OnScreenConsole(float ki, Input_Handler* in, EventFeedback* fb,int window_width, int window_height) :OnScreenConsole()
{
	this->feedback = fb;
	this->input = in;
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

void OnScreenConsole::actOnChange(eventType et)
{
	switch (et){

	case KEYRELEASED:
		if (insert_mode)
		{
			if (input->last_input == '\x1')
			{
				insert_mode = !insert_mode;
				interpreteInput(clear());
				break;
			}
			if (!current_input) {
				current_input = new OnScreenMessage(std::string()+input->last_input);
				out(current_input);
			}
			else {
				current_input->addToMessage(std::string() + input->last_input);
			}
		}
		else
		{
			switch (input->last_input)
			{
			case 'I':
				in();
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;

	}
}

bool OnScreenConsole::isInInsertMode()
{
	return insert_mode;
}

void OnScreenConsole::interpreteInput(std::string in)
{
	if (!in.compare("QUIT") || !in.compare("Q"))
	{
		feedback->quitgame = true;
	}
	else if (!in.compare("RESTART") || !in.compare("R"))
	{
		feedback->restart = true;
		feedback->quitgame = false;
	}
	else if (!in.compare("ABC"))
	{
		out(new OnScreenMessage("D"));
	}
	else if (!in.compare("YOU STINK"))
	{
		out(new OnScreenMessage("STFU!!!!!"));
	}
}
