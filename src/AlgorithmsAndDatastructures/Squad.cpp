#include "Squad.h"
#include <glm\gtx\quaternion.hpp>

Squad::Squad()
{
}


Squad::~Squad()
{
}

glm::quat Squad::step(int current_p, GLfloat t)
{
	glm::quat &p1 = quaternions[current_p];
	glm::quat &p2 = quaternions[current_p + 1];

	return glm::squad(p1, p2, intermediate[current_p], intermediate[current_p + ((current_p == intermediate.size()-1) ? 0 : 1)], t);
}

void Squad::add_quat(GLfloat angle, glm::vec3 axis)
{
	Slerp::add_quat(angle, axis);
	if (quaternions.size() > 1) {
		int current = quaternions.size()-1;
		intermediate.push_back(glm::intermediate( quaternions[current - ((quaternions.size() == 2) ? 1 : 2)], quaternions[current - 1], quaternions[current]));
	}
}

void Squad::add_quat(glm::quat q) {
	Slerp::add_quat(q);
	if (quaternions.size() > 1) {
		int current = quaternions.size() - 1;
		intermediate.push_back(glm::intermediate(quaternions[current - ((quaternions.size() == 2) ? 1 : 2)], quaternions[current - 1], quaternions[current]));
	}
}
