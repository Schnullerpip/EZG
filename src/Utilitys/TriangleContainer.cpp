#include "TriangleContainer.h"

//udeful helper functions
float getx(float* triangle) { return *triangle; }
float gety(float* triangle) { return *(triangle + 1); }
float getz(float* triangle) { return *(triangle + 2); }
bool greaterThan(float a, float b) { return a > b; }
bool smallerThan(float a, float b) { return !greaterThan(a, b); }

TriangleContainer::TriangleContainer(float* triangleLocation, Shape* prim, int offset):primitive(prim), triangle(triangleLocation), offset(offset)
{
}

float* TriangleContainer::A()const
{
	return triangle;
}
float* TriangleContainer::B()const
{
	return triangle + offset;
}
float* TriangleContainer::C()const
{
	return triangle + offset*2;
}


float TriangleContainer::getX(float* triangle) const
{
	return *triangle;
}
float TriangleContainer::getY(float* triangle) const
{
	return *(triangle + 1);
}
float TriangleContainer::getZ(float* triangle) const
{
	return *(triangle + 2);
}


bool TriangleContainer::equals(TriangleContainer* tc)const
{
	 return (A() == tc->A() && B() == tc->B() && C() == tc->C());
}


float TriangleContainer::most(float(*genericGetter)(float*), bool (*generic_comparision)(float a, float b), float offset) const
{
		float target = genericGetter(A()) + offset, tmp;
		if (generic_comparision(tmp = genericGetter(B()) + offset, target)) { target = tmp; }
		if (generic_comparision(tmp = genericGetter(C()) + offset, target)) { target = tmp; }

		return target;
	}

/** returns a bounding box after processing the actual bounds*/
BoundingBox TriangleContainer::getBoundingBox() const
{
	float lowest_x = most(getx, smallerThan, primitive->getPosition().x);
	float highest_x = most(getx, greaterThan, primitive->getPosition().x);

	float lowest_y = most(gety, smallerThan, primitive->getPosition().y);
	float highest_y = most(gety, greaterThan, primitive->getPosition().y);

	float lowest_z = most(getz, smallerThan, primitive->getPosition().z);
	float highest_z = most(getz, greaterThan, primitive->getPosition().z);

	return BoundingBox(lowest_x, highest_x, lowest_y, highest_y, lowest_z, highest_z, primitive->getPosition());
}

float* TriangleContainer::getTrianglePtr() const
{
	return triangle;
}

Shape* TriangleContainer::getPrimitive() const
{
	return primitive;
}

Point3D TriangleContainer::getMidPoint() const
{
	//TODO maybe save those results, since they're needed both by getMidPoint() and getBoundingBox()
	float mid_x = most(getx, greaterThan) - most(getx, smallerThan) / 2;
	float mid_y = most(gety, greaterThan) - most(gety, smallerThan) / 2;
	float mid_z = most(getz, greaterThan) - most(getz, smallerThan) / 2;
	return Point3D(mid_x, mid_y, mid_z);
}

TriangleContainer::~TriangleContainer()
{
}
