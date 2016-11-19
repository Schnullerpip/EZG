#pragma once
#include <vector>
#include <GL\glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class Slerp
{
protected:
	std::vector<glm::quat> quaternions;
	GLfloat t = 0;
	GLfloat total_t = 0;
public:
	GLfloat interval = 0.0003f;
	Slerp();
	~Slerp();
	virtual glm::quat step(int current_quat, GLfloat t);
	virtual glm::quat step();//dont use it! buggy

	virtual void add_quat(GLfloat angle, glm::vec3 axis);
	virtual void add_quat(glm::quat q);
};