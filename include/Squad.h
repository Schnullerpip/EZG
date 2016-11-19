#pragma once
#include "Slerp.h"
class Squad : public Slerp
{
	std::vector<glm::quat> intermediate;
public:
	Squad();
	~Squad();
	glm::quat step(int, GLfloat)override;
	virtual void add_quat(GLfloat angle, glm::vec3 axis)override;
	virtual void add_quat(glm::quat q) override;
};

