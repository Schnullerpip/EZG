#include "BoundingBox.h"

void BoundingBox::recalculate()
{
	//recalculate the bounds
	width = to_x - from_x;
	height = to_y - from_y;
	depth = to_z - from_z;

	//recalculate the origin
	origin.x = width / 2;
	origin.y = height / 2;
	origin.z = depth / 2;
}

BoundingBox::BoundingBox(float fromX, float toX, float fromY, float toY, float fromZ, float toZ, glm::vec3 origin):from_x(fromX), to_x(toX), from_y(fromY), to_y(toY), from_z(fromZ), to_z(toZ), origin(origin)
{
	recalculate();
}

void BoundingBox::expand(BoundingBox& bb)
{
	//expand the bounds - if neccessary replace the existing bounds by the new bb one's
	from_x = (bb.from_x < from_x) ? bb.from_x : from_x;
	to_x = (bb.to_x > to_x) ? bb.to_x : to_x;

	from_y = (bb.from_y < from_y) ? bb.from_y : from_y;
	to_y = (bb.to_y > to_y) ? bb.to_y : to_y;

	from_z = (bb.from_z < from_z) ? bb.from_z : from_z;
	to_z = (bb.to_z > to_z) ? bb.to_z : to_z;

	//recalculate width height length
	recalculate(); 
}

/*
 * returns 0 - width
 * returns 1 - height
 * returns 2 - depth
 */
int BoundingBox::longestAxis() const
{
	if (height > width && depth < height) return 1;
	if (depth > width) { return 2; }
	return 0;
}

glm::vec3 BoundingBox::getPosition() const
{
	return origin;
}

BoundingBox::~BoundingBox()
{
}
