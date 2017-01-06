#pragma once
#include <glm/glm.hpp>
#include "Camera.h"

#define TMAX 500;

class Ray
{
	glm::vec3 origin;
	glm::vec3 direction, inv_direction;
	float tMAX = TMAX;
	float t_intersection = -1;
public:
	Ray();
	explicit Ray(glm::vec3 origin);
	Ray(glm::vec3 origin, glm::vec3 direction);
	explicit Ray(Camera* cam);
	Ray(const Ray& ray);

	glm::vec3 beam(float t)const;

	~Ray();

	int sign[3];
	void setTMax(float t);
	float getTMax() const;
	glm::vec3 Origin() const;
	glm::vec3 Direction() const;
	glm::vec3 InvDirection() const;
	void setT(float t);
	const float* T()const;
};
