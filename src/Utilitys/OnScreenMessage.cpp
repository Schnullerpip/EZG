#include "OnScreenMessage.h"


OnScreenMessage::OnScreenMessage(std::string m, glm::vec3 c, float s):message(m), color(c), scale(s)
{
}


OnScreenMessage::~OnScreenMessage()
{
}

void OnScreenMessage::addToMessage(std::string s)
{
	message += s;
}
