#pragma once
#include <glm\glm.hpp>
#include <GL\glew.h>
#include "Shape.h"
using glm::vec3;
class Triangle : public Shape
{
protected:
	static GLuint VAO;
	GLfloat vertices[9];
public:
	Triangle(vec3 P0 = vec3(0.f, 0.5f, 0), vec3 P1 = vec3(-0.5f, -0.5f, 0), vec3 P2 = vec3(0.5f, -0.5f, 0));
	virtual ~Triangle();
	//void draw(glm::mat4* model)override;
	void render() override;
};

