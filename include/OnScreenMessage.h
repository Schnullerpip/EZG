#pragma once
#include <string>
#include <glm/glm.hpp>

class OnScreenMessage
{
public:
	std::string message;
	glm::vec3 color;
	float scale;
	OnScreenMessage(std::string, glm::vec3 = glm::vec3(0.5f, 0.8f, 0.3f), float = 0.3f);
	~OnScreenMessage();

	void addToMessage(std::string s);
};

