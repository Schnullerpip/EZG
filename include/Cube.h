#pragma once
#include "Shape.h"
#include <vector>

class Cube : public Shape
{
protected:
public:
	Cube(Shader* s, glm::vec3 pos_glo = glm::vec3(0.f, 0.f, 0.f), GLfloat width = 1.f, GLfloat height = 1.f, GLfloat depth = 1.f, glm::vec3 pos_loc = glm::vec3(0.f, 0.f, 0.f));
	virtual void draw() override;
	~Cube();
};

