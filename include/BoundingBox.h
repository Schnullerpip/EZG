#pragma once
#include <glm/glm.hpp>

/**A container pointing to 1 to n Triangles, also storing the bound's lengths and so on*/
class BoundingBox
{
	//the bounds of the triangle
	glm::vec3 origin;
	float width, height, depth;
	float from_x, to_x, from_y, to_y, from_z, to_z;
	void recalculate();
public:
	BoundingBox() :BoundingBox(0, 0, 0, 0, 0, 0, glm::vec3(0, 0, 0)) {}
	BoundingBox(float fromX, float toX, float fromY, float toY, float fromZ, float toZ, glm::vec3 origin);

	void expand(BoundingBox* bb);
	int longestAxis()const;
	glm::vec3 getPosition()const;

	float Width()const;
	float Height()const;
	float Depth()const;

	float FromX()const;
	float FromY()const;
	float FromZ()const;
	float ToX()const;
	float ToY()const;
	float ToZ()const;


	~BoundingBox();
};