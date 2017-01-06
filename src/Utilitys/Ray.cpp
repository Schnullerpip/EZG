#include "Ray.h"



Ray::Ray():Ray(glm::vec3())
{
}


Ray::Ray(glm::vec3 origin):Ray(origin, glm::vec3())
{
}

Ray::Ray(glm::vec3 origin, glm::vec3 d):origin(origin), direction(d)
{
	inv_direction = glm::vec3(1/d.x, 1/d.y, 1/d.z);

  	sign[0] = (inv_direction.x < 0);
  	sign[1] = (inv_direction.y < 0);
  	sign[2] = (inv_direction.z < 0);
}

Ray::Ray(Camera* cam):Ray(cam->pos, cam->front)
{
}

Ray::Ray(const Ray& ray)
{
	origin = ray.origin;
	direction = ray.direction;
	inv_direction = ray.inv_direction;
	sign[0] = ray.sign[0]; sign[1] = ray.sign[1]; sign[2] = ray.sign[2];
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

glm::vec3 Ray::InvDirection() const
{
	return inv_direction;
}

void Ray::setT(float t)
{
	t_intersection = t;
}

const float* Ray::T() const
{
	if (t_intersection > 0)return &t_intersection;
	return nullptr;
}
