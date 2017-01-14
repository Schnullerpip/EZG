#pragma once
#include "BoundingBox.h"
#include "Shape.h"
#include "Point3D.h"

class TriangleContainer
{
	Shape* primitive; //the primitive that owns this triangle
	float* triangle;
	int offset;

	//useful props
public:
	glm::vec3 world_A, world_B, world_C;
	float* A()const;
	float* B()const;
	float* C()const;

	float getX(float* triangle) const;
	float getY(float* triangle) const;
	float getZ(float* triangle) const;
	float getXLocal(float* triangle) const;
	float getYLocal(float* triangle) const;
	float getZLocal(float* triangle) const;

	BoundingBox getBoundingBox()const;
	float* getTrianglePtr()const;
	Shape* getPrimitive()const;
	/**
	 * \get the most (smallest, greatest..) of all the points (A, B, C) and determine which coordinate value (x, y, or z) are compared
	 * \param genericGetter - determines whether the x y or z coordinate are fetched
	 * \param generic_comparision - compares in a generic way
	 * \return  the smallest/greates whatever value according to the generic_comparision and what is fetched
	 */
	float TriangleContainer::most(float(genericGetter)(const glm::vec3*), bool(*generic_comparision)(float a, float b), float offset = 0)const;
	bool equals(TriangleContainer* tc) const;
	Point3D getMidPoint()const;
	bool hit(Ray* r)const;
	bool intersects(Ray*) const;
	TriangleContainer(float* triangleLocation, Shape* prim, int offset = 0);
	TriangleContainer(glm::vec3 A, glm::vec3 B, glm::vec3 C, Shape* primitive);
	~TriangleContainer();
};
