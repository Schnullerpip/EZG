#include "BoundingBox.h"

void BoundingBox::recalculate()
{
	//recalculate the bounds
	width = max.x - min.x;
	height = max.y - min.y;
	depth = max.z - min.z;

	//recalculate the origin
	origin.x = min.x + width / 2;
	origin.y = min.y + height / 2;
	origin.z = min.z + depth / 2;

	//recalculate the min and max points (important for intersection testing)
	min.x = min.x;
	min.y = min.y;
	min.z = min.z;

	max.x = max.x;
	max.y = max.y;
	max.z = max.z;
}

BoundingBox::BoundingBox(float fromX, float toX, float fromY, float toY, float fromZ, float toZ, glm::vec3 origin)
{
	min.x = fromX;
	min.y = fromY;
	min.z = fromZ;

	max.x = toX;
	max.y = toY;
	max.z = toZ;

	parameters[0] = min;
	parameters[1] = max;

	this->origin = origin;

	recalculate();
}

void BoundingBox::expand(BoundingBox& bb)
{
	//expand the bounds - if neccessary replace the existing bounds by the new bb one's
	min.x = (bb.min.x < min.x) ? bb.min.x : min.x;
	max.x = (bb.max.x > max.x) ? bb.max.x : max.x;

	min.y = (bb.min.y < min.y) ? bb.min.y : min.y;
	max.y = (bb.max.y > max.y) ? bb.max.y : max.y;

	min.z = (bb.min.z < min.z) ? bb.min.z : min.z;
	max.z = (bb.max.z > max.z) ? bb.max.z : max.z;

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

bool BoundingBox::hit(Ray* r) const
{
  float tmin, tmax, tymin, tymax, tzmin, tzmax;
    
    if (r->Direction().x >= 0) {
        tmin = (min.x - r->Origin().x) / r->Direction().x;
        tmax = (max.x - r->Origin().x) / r->Direction().x;
    }
    else {
        tmin = (max.x - r->Origin().x) / r->Direction().x;
        tmax = (min.x - r->Origin().x) / r->Direction().x;
    }
    if (r->Direction().y >= 0) {
        tymin = (min.y - r->Origin().y) / r->Direction().y;
        tymax = (max.y - r->Origin().y) / r->Direction().y;
    }
    else {
        tymin = (max.y - r->Origin().y) / r->Direction().y;
        tymax = (min.y - r->Origin().y) / r->Direction().y;
    }
    if ( (tmin > tymax) || (tymin > tmax) )
        return false;
    
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;
    if (r->Direction().z >= 0) {
        tzmin = (min.z - r->Origin().z) / r->Direction().z;
        tzmax = (max.z - r->Origin().z) / r->Direction().z;
    }
    else {
        tzmin = (max.z - r->Origin().z) / r->Direction().z;
        tzmax = (min.z - r->Origin().z) / r->Direction().z;
    }
    if ( (tmin > tzmax) || (tzmin > tmax) )
        return false;
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;
    
    return ( (tmin < r->getTMax()) && (tmax > 0) );
}

bool BoundingBox::surrounds(glm::vec3& point) const
{
	float e = 0.01f;
	if (point.x - e <= max.x && point.x + e>= min.x)
	{
		if (point.y - e <= max.y && point.y + e >= min.y)
		{
			if (point.z - e <= max.z && point.z + e >= min.z)
			{
				return true;
			} 
		} 
	}
	return false;
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
	return min.x;
}

float BoundingBox::FromY() const
{
	return min.y;
}

float BoundingBox::FromZ() const
{
	return min.z;
}

float BoundingBox::ToX() const
{
	return max.x;
}

float BoundingBox::ToY() const
{
	return max.y;
}

float BoundingBox::ToZ() const
{
	return max.z;
}

void BoundingBox::setFromX(float m)
{
	min.x = m;
}

void BoundingBox::setFromY(float m)
{
	min.y = m;
}

void BoundingBox::setFromZ(float m)
{
	min.z = m;
}

void BoundingBox::setToX(float m)
{
	max.x = m;
}

void BoundingBox::setToY(float m)
{
	max.y = m;
}

void BoundingBox::setToZ(float m)
{
	max.z = m;
}

BoundingBox::~BoundingBox()
{
}
