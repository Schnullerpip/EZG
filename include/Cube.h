#pragma once
#include "Shape.h"

class Cube : public Shape
{
protected:
public:
	Cube(Shader* s, glm::vec3 pos_glo = glm::vec3(0.f, 0.f, 0.f), GLfloat width = 1.f, GLfloat height = 1.f, GLfloat depth = 1.f, glm::vec3 pos_loc = glm::vec3(0.f, 0.f, 0.f));
	Cube(Shader* s, glm::vec3 pos_glo,
		glm::vec3 tlf = glm::vec3(-.5f, .5f, .5f),
		glm::vec3 trf = glm::vec3(.5f, .5f, .5f),
		glm::vec3 blf = glm::vec3(-.5f, -.5f, .5f),
		glm::vec3 brf = glm::vec3(.5f, -.5f, .5f),
		glm::vec3 tlb = glm::vec3(-.5f, .5f, -.5f),
		glm::vec3 trb = glm::vec3(.5f, .5f, -.5f),
		glm::vec3 blb = glm::vec3(-.5f, -.5f, -.5f),
		glm::vec3 brb = glm::vec3(.5f, -.5f, -.5f));
	virtual void draw() override;
	~Cube();
};

