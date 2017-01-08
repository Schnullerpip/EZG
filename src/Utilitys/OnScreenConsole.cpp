#include "OnScreenConsole.h"
#include "Input_handler.h"
#include <GLFW/glfw3.h>

const int num_targets = 6;
int targets[num_targets] = {
	GL_MULTISAMPLE_FILTER_HINT_NV,
	GL_LINE_SMOOTH_HINT,
	GL_PERSPECTIVE_CORRECTION_HINT,
	GL_POINT_SMOOTH_HINT,
	GL_POLYGON_SMOOTH_HINT,
	GL_TEXTURE_COMPRESSION_HINT,
};

const char* target_strings[num_targets] = {
	"GL_MULTISAMPLE_FILTER_HINT_NV",
	"GL_LINE_SMOOTH_HINT",
	"GL_PERSPECTIVE_CORRECTION_HINT",
	"GL_POINT_SMOOTH_HINT",
	"GL_POLYGON_SMOOTH_HINT",
	"GL_TEXTURE_COMPRESSION_HINT",
};



std::string OnScreenConsole::clear()
{
	std::string retVal = current_input->message;
	last_command = retVal;
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
	if (!msg && current_input)
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

void OnScreenConsole::out(std::string message)
{
	out(new OnScreenMessage(message));
}

void OnScreenConsole::registerCommand(bool* b, const char* command, const char* out_message)
{
	references.push_back(std::make_tuple(b, command, out_message));
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
	delete textRenderer;
}

void OnScreenConsole::in()
{
	current_input = new OnScreenMessage("$>");
	out();
	insert_mode = true;
}

void OnScreenConsole::actOnChange(eventType et)
{
	//std::cout << input->last_input << std::endl;
	switch (et){

	case KEYRELEASED:
		if (insert_mode)
		{
			if (input->last_input == '\x1') //return
			{
				insert_mode = !insert_mode;
				interpreteInput(clear());
				break;
			}
			if (input->last_input == '\t') //arrow up
			{
				current_input->message = last_command;
				break;
			}
			if (input->last_input == '\x2') //tab
			{
				proposal(current_input->message);
				break;
			}
			if (input->last_input == '\x3') //delete
			{
				if (current_input->message.size() > 2)
					current_input->message = current_input->message.substr(0, current_input->message.size() - 1);
				break;
			}
			current_input->addToMessage(std::string() + input->last_input);
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

bool OnScreenConsole::isInInsertMode() const
{
	return insert_mode;
}


	std::string isSuported(const char* ext)
	{
		std::stringstream ss;
		if (glewIsSupported(ext))
		{
			ss << ext << " is supported!";
		}
		else
		{
			ss << ext << " is NOT supported!";
		}

		std::string retVal = ss.str();
		return retVal;
	};


void OnScreenConsole::interpreteInput(std::string input)
{
	std::string in = input.substr(2, input.size());
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
	else if (in.find("SAMPLES") != std::string::npos)
	{
		std::stringstream ss(in);
		ss.ignore(in.size(), ' ');
		ss >> feedback->number_samples;
		ss.clear();
		ss << "[samples]::set to " << feedback->number_samples;
		std::string out_string(ss.str());
		out(new OnScreenMessage(out_string));
	}
	else if (in.find("HINT") != std::string::npos)
	{
		bool all = false;
		out(isSuported("GL_NV_multisample_filter_hint"));
		out(isSuported("GL_NV_framebuffer_multisample_coverage"));

		int target_idx = -1, mode = -1;
		std::stringstream ss(in);
		std::string target_s, mode_s;
		ss >> target_s >> target_s; //now has target
		ss >> mode_s; //now has mode 

		if (mode_s == "NICEST") {
			mode = GL_NICEST;
		}
		else if (mode_s == "FASTEST") {
			mode = GL_FASTEST;
		}
		else {
			mode_s = "DONT_CARE";
			mode = GL_DONT_CARE;
		}

		auto apply_mode = [&](int t, std::string ts, int m, std::string ms)
		{
			glHint(t, m);
			feedback->hints.push_back(std::pair<int, int>(t, m));
			out(std::string("[") + ts + "]::set to -> " + ms);
		};

		for (int i = 0; i < num_targets; ++i)
		{
			if (!target_s.compare(target_strings[i]))
			{
				target_idx = i;
				break;
			}
		}
		if (target_idx == -1)
		{
			if (target_s == "ALL")
			{
				ss >> mode_s; //now has mode
				for (int i = 0; i < num_targets; ++i)
				{
					apply_mode(targets[i], std::string(target_strings[i]), mode, mode_s);
				}
			}
			else { return; }
		}

		int target_int = targets[target_idx];
		apply_mode(target_idx, target_s, mode, mode_s);
	}
	//possibility to configure different modes
	else if (!in.compare("CSAA"))
	{
		//TODO
		out(new OnScreenMessage("[AA]::mode set to CSAA -> not supported yet"));
	}
	else if (!in.compare("FSAA"))
	{
		//TODO
		out(new OnScreenMessage("[AA]::mode set to FSAA -> not supported yet"));
	}
	else if (!in.compare("MSAA"))
	{
		//TODO
		out(new OnScreenMessage("[AA]::mode set to MSAA"));
	}
	else
	{
		for (auto c : references) //the user defined commands - if one of them were found trigger the according bool
		{
			if (in == std::get<1>(c))
			{
				*std::get<0>(c) = !*std::get<0>(c);
				out(std::get<2>(c));
				return;
			}
		}

		if (in != "")
		{
			out(new OnScreenMessage("unknown command"));
		}
	}
}


void OnScreenConsole::proposal(std::string s) const
{
	static int i = 0;
	if (s.find("HINT") != std::string::npos)
	{
		current_input->message = std::string("$>HINT ") + target_strings[i++] + " ";
		if (i >= 5)i = 0;
	}
}
