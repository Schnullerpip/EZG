#pragma once
#include "Shape.h"

class SimpleCube : public Shape
{
protected:
public:
	SimpleCube();
	SimpleCube(Shader* s, glm::vec3 pos_glo = glm::vec3(0.f, 0.f, 0.f), GLfloat width = 1.f, GLfloat height = 1.f, GLfloat depth = 1.f, glm::vec3 pos_loc = glm::vec3(0.f, 0.f, 0.f));
	~SimpleCube();
	GLuint ibo_elements;
	void draw() override;
};

