#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Shader.h"
#include "Texture.h"

class Shape
{
protected:
	GLuint VBO;
	std::vector<GLfloat> vertices;
public:
	GLuint VAO;
	glm::vec3 position_global;
	glm::vec4 rotation;
	glm::vec4 scalation;

	glm::vec3 position_local;
	Shader* shader = nullptr;
	Texture* texture = nullptr;
	Texture* normalMap = nullptr;
	Shape() {
		glGenBuffers(1, &VBO);
	}
	inline glm::vec3 getPosition() const { return position_global; }
	inline glm::vec3 getPositionLocal() const { return position_local; }
	virtual void draw() = 0;
	virtual ~Shape() {
		if (shader)delete shader;
		if (texture)delete texture;
		glDeleteBuffers(1, &VBO);
	}
};

