#pragma once
#include <glm/glm.hpp>
#include "Camera.h"

#define TMAX 500;

class Ray
{
	glm::vec3 origin;
	glm::vec3 direction;
	float tMAX = TMAX;
public:
	Ray();
	explicit Ray(glm::vec3 origin);
	Ray(glm::vec3 origin, glm::vec3 direction);
	explicit Ray(Camera* cam);
	glm::vec3 beam(float t)const;

	~Ray();

	void setTMax(float t);
	float getTMax() const;
	glm::vec3 Origin() const;
	glm::vec3 Direction() const;
};
