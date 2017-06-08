#pragma once
#include "Shape.h"
class Pyramid : public Shape
{
protected:
	static GLuint VAO;
public:
	Pyramid(glm::vec3 pos_glo = glm::vec3(0.f, 0.f, 0.f), GLfloat width = 1.f, GLfloat height = 1.f, GLfloat depth = 1.f, glm::vec3 pos_loc = glm::vec3(0,0,0));
	//virtual void draw(glm::mat4* model)override;
	virtual void render() override;
	~Pyramid();
};

