#pragma once
#include <glm/glm.hpp>
#include "Ray.h"

/**A container pointing to 1 to n Triangles, also storing the bound's lengths and so on*/
class Shape;
class BoundingBox
{
	//the bounds of the triangle
	glm::vec3 origin;
	float width, height, depth;
	glm::vec3 min, max;
	glm::vec3 parameters[2];
public:
	int split_axis;
	float median;
	BoundingBox():BoundingBox(0, 0, 0, 0, 0, 0, glm::vec3(0, 0, 0)) {}
	BoundingBox(float fromX, float toX, float fromY, float toY, float fromZ, float toZ, glm::vec3 origin);

	void recalculate();
	void expand(BoundingBox& bb);
	int longestAxis()const;
	glm::vec3 getPosition()const;
	bool hit(Ray*) const;

	bool surrounds(glm::vec3&)const;

	float Width()const;
	float Height()const;
	float Depth()const;

	float FromX()const;
	float FromY()const;
	float FromZ()const;
	float ToX()const;
	float ToY()const;
	float ToZ()const;

	void setFromX(float m);
	void setFromY(float m);
	void setFromZ(float m);
	void setToX(float m);
	void setToY(float m);
	void setToZ(float m);

	~BoundingBox();
};