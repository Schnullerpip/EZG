#include "TriangleContainer.h"

//udeful helper functions
float getx(float* triangle) { return *triangle; }
float gety(float* triangle) { return *(triangle + 1); }
float getz(float* triangle) { return *(triangle + 2); }
bool greatest(float a, float b) { return a > b; }
bool smallest(float a, float b) { return !greatest(a, b); }

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


/**
 * \brief gives the worldposition of a primitive's triangle
 * \param triangle - the pointer to the GLfloat array, that holds all the vertice data of the cube
 * assumes, that the data is ordered like this -> [x, y, z, some other shit...blabla.., x, y, z, falskdjföa]
 * \return returns the resolved pointer to the triangle plus the primitive position
 */
float TriangleContainer::getX(float* triangle) const
{
	return *triangle + primitive->getPosition().x;
}
/**
 * \brief gives the worldposition of a primitive's triangle
 * \param triangle - the pointer to the GLfloat array, that holds all the vertice data of the cube
 * assumes, that the data is ordered like this -> [x, y, z, some other shit...blabla.., x, y, z, falskdjföa]
 * \return returns the resolved pointer to the triangle plus the primitive position
 */
float TriangleContainer::getY(float* triangle) const
{
	return *(triangle + 1) + primitive->getPosition().y;
}
/**
 * \brief gives the worldposition of a primitive's triangle
 * \param triangle - the pointer to the GLfloat array, that holds all the vertice data of the cube
 * assumes, that the data is ordered like this -> [x, y, z, some other shit...blabla.., x, y, z, falskdjföa]
 * \return returns the resolved pointer to the triangle plus the primitive position
 */
float TriangleContainer::getZ(float* triangle) const
{
	return *(triangle + 2) + primitive->getPosition().z;
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
BoundingBox* TriangleContainer::getBoundingBox() const
{
	float lowest_x = most(getx, smallest, primitive->getPosition().x);
	float highest_x = most(getx, greatest, primitive->getPosition().x);

	float lowest_y = most(gety, smallest, primitive->getPosition().y);
	float highest_y = most(gety, greatest, primitive->getPosition().y);

	float lowest_z = most(getz, smallest, primitive->getPosition().z);
	float highest_z = most(getz, greatest, primitive->getPosition().z);

	return new BoundingBox(lowest_x, highest_x, lowest_y, highest_y, lowest_z, highest_z, primitive->getPosition());
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
	float mid_x = most(getx, greatest) - most(getx, smallest) / 2;
	float mid_y = most(gety, greatest) - most(gety, smallest) / 2;
	float mid_z = most(getz, greatest) - most(getz, smallest) / 2;
	return Point3D(mid_x, mid_y, mid_z);
}

TriangleContainer::~TriangleContainer()
{
}
