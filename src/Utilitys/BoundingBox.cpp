#include "BoundingBox.h"

void BoundingBox::recalculate()
{
	//recalculate the bounds
	width = to_x - from_x;
	height = to_y - from_y;
	depth = to_z - from_z;

	//recalculate the origin
	origin.x = from_x + width / 2;
	origin.y = from_y + height / 2;
	origin.z = from_z + depth / 2;
}

BoundingBox::BoundingBox(float fromX, float toX, float fromY, float toY, float fromZ, float toZ, glm::vec3 origin):from_x(fromX), to_x(toX), from_y(fromY), to_y(toY), from_z(fromZ), to_z(toZ), origin(origin)
{
	recalculate();
}

void BoundingBox::expand(BoundingBox* bb)
{
	//expand the bounds - if neccessary replace the existing bounds by the new bb one's
	from_x = (bb->from_x < from_x) ? bb->from_x : from_x;
	to_x = (bb->to_x > to_x) ? bb->to_x : to_x;

	from_y = (bb->from_y < from_y) ? bb->from_y : from_y;
	to_y = (bb->to_y > to_y) ? bb->to_y : to_y;

	from_z = (bb->from_z < from_z) ? bb->from_z : from_z;
	to_z = (bb->to_z > to_z) ? bb->to_z : to_z;

	//recalculate width height length
	recalculate(); 
}

/*
 * returns 0 - width(x)
 * returns 1 - height(y)
 * returns 2 - depth(z)
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

float BoundingBox::Width() const
{
	return width;
}

float BoundingBox::Height() const
{
	return height;
}

float BoundingBox::Depth() const
{
	return depth;
}

float BoundingBox::FromX() const
{
	return from_x;
}

float BoundingBox::FromY() const
{
	return from_y;
}

float BoundingBox::FromZ() const
{
	return from_z;
}

float BoundingBox::ToX() const
{
	return to_x;
}

float BoundingBox::ToY() const
{
	return to_y;
}

float BoundingBox::ToZ() const
{
	return to_z;
}

void BoundingBox::setFromX(float m)
{
	from_x = m;
}

void BoundingBox::setFromY(float m)
{
	from_y = m;
}

void BoundingBox::setFromZ(float m)
{
	from_z = m;
}

void BoundingBox::setToX(float m)
{
	to_x = m;
}

void BoundingBox::setToY(float m)
{
	to_y = m;
}

void BoundingBox::setToZ(float m)
{
	to_z = m;
}

BoundingBox::~BoundingBox()
{
}
