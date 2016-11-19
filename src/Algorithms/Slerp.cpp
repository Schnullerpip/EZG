#include "Slerp.h"
#include <iostream>

Slerp::Slerp()
{
}


Slerp::~Slerp()
{
}

glm::quat Slerp::step(int current_quat, GLfloat t)
{
	glm::quat& q1 = quaternions[current_quat];
	glm::quat& q2 = (current_quat < quaternions.size()) ? quaternions[current_quat + 1] : quaternions[quaternions.size() - 1];
	glm::quat retVal = glm::slerp(q1, q2, t);
	return retVal;
}

glm::quat Slerp::step()
{
	int q_position = (int)total_t;
	glm::quat& q1 = quaternions[q_position];
	glm::quat& q2 = (q_position < quaternions.size()-1) ? quaternions[q_position + 1] : quaternions[quaternions.size() - 1];
	glm::quat retVal = glm::slerp(q1, q2, t);

	if ((t += interval) >= 1) t = 0;
	if ((total_t += interval) > quaternions.size() - 1)total_t = 0;
	return retVal;
}

void Slerp::add_quat(GLfloat angle, glm::vec3 axis)
{
  quaternions.push_back(glm::angleAxis(glm::radians(angle), axis));
}

void Slerp::add_quat(glm::quat q) {
	quaternions.push_back(q);
}
