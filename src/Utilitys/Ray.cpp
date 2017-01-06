#include "Ray.h"



Ray::Ray():Ray(glm::vec3())
{
}


Ray::Ray(glm::vec3 origin):Ray(origin, glm::vec3())
{
}

Ray::Ray(glm::vec3 origin, glm::vec3 direction):origin(origin), direction(direction)
{
}

Ray::Ray(Camera* cam):origin(cam->pos), direction(cam->front)
{
}

glm::vec3 Ray::beam(float t) const
{
	return (origin + direction * t);
}

Ray::~Ray()
{
}

void Ray::setTMax(float t)
{
	tMAX = t;
}

float Ray::getTMax() const
{
	return tMAX;
}

glm::vec3 Ray::Origin() const
{
	return origin;
}

glm::vec3 Ray::Direction() const
{
	return direction;
}
