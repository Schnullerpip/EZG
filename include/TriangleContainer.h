#pragma once
#include "BoundingBox.h"
#include "Shape.h"
#include "../Point3D.h"

class TriangleContainer
{
	Shape* primitive; //the primitive that owns this triangle
	float* triangle;
	int offset;

	//useful props
public:
	float* A()const;
	float* B()const;
	float* C()const;

	float getX(float* triangle) const;
	float getY(float* triangle) const;
	float getZ(float* triangle) const;

	BoundingBox	getBoundingBox()const;
	Shape* getPrimitive()const;
	/**
	 * \get the most (smallest, greatest..) of all the points (A, B, C) and determine which coordinate value (x, y, or z) are compared
	 * \param genericGetter - determines whether the x y or z coordinate are fetched
	 * \param generic_comparision - compares in a generic way
	 * \return  the smallest/greates whatever value according to the generic_comparision and what is fetched
	 */
	float TriangleContainer::most(float(genericGetter)(float*), bool(*generic_comparision)(float a, float b), float offset = 0)const;
	bool equals(TriangleContainer* tc) const;
	Point3D getMidPoint()const;
	TriangleContainer(float* triangleLocation, Shape* prim, int offset = 0);
	~TriangleContainer();
};
