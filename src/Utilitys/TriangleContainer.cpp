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
	float mid_x = most(getx, greatest, primitive->getPosition().x) - most(getx, smallest, primitive->getPosition().x) / 2;
	float mid_y = most(gety, greatest, primitive->getPosition().y) - most(gety, smallest, primitive->getPosition().y) / 2;
	float mid_z = most(getz, greatest, primitive->getPosition().z) - most(getz, smallest, primitive->getPosition().z) / 2;
	return Point3D(mid_x, mid_y, mid_z);
}

glm::vec3 toVec(const TriangleContainer* tr, float* t)
{
	return glm::vec3(tr->getX(t), tr->getY(t), tr->getZ(t));
}

#define EPSILON 0.000001
bool TriangleContainer::hit(Ray* r) const
{
	glm::vec3 e1, e2;  //Edge1, Edge2
	glm::vec3 P, Q, T, V1 = toVec(this, A()), V2 = toVec(this, B()), V3 = toVec(this, C());
	float det, inv_det, u, v;
	float t;

	//Find vectors for two edges sharing V1
	e1 = V2 - V1;
	e2 = V3 - V1;
	//Begin calculating determinant - also used to calculate u parameter
	P = glm::cross(r->Direction(), e2);
	//if determinant is near zero, ray lies in plane of triangle or ray is parallel to plane of triangle
	det = glm::dot(e1, P);
	//NOT CULLING
	if (det > -EPSILON && det < EPSILON) return false;
	inv_det = 1.f / det;

	//calculate distance from V1 to ray origin
	T = r->Origin() - V1;

	//Calculate u parameter and test bound
	u = inv_det * glm::dot(T, P);
	//The intersection lies outside of the triangle
	if (u < 0.f || u > 1.f) return false;

	//Prepare to test v parameter
	Q = glm::cross(T, e1);

	//Calculate V parameter and test bound
	v = glm::dot(r->Direction(), e1);
	//The intersection lies outside of the triangle
	if (v < 0.f || u + v  > 1.f) return false;

	t = glm::dot(e2, Q) * inv_det;

	if (t > EPSILON) { //ray intersection
		r->setT(t);
		return true;
	}

	// No hit, no win
	return false;
}
bool TriangleContainer::intersects(Ray *ray) const
{
	glm::vec3 edge1 = toVec(this, B()) - toVec(this, A());
    glm::vec3 edge2 = toVec(this, C()) - toVec(this, A());
    
    glm::vec3 q = glm::cross(ray->Direction(), edge2);
    float det = glm::dot(edge1, q);
    
    if(det > -EPSILON && det < EPSILON)
        return false;
    
    float inv_det = 1.0f/det;
    
    glm::vec3 s = ray->Origin() - toVec(this, A());
    
    float u = glm::dot(s, q) * inv_det;
    
    if(u < 0.0f || u > 1.0f)
        return false;
    
    glm::vec3 r = glm::cross(s, edge1);
    
    float v = glm::dot(ray->Direction(), r) * inv_det;
    if(v < 0.0f || u + v > 1.0f)
        return false;
    
    float t = glm::dot(edge2, r) * inv_det;
    
    if(t > EPSILON)
    {
        ray->setT(t);
        return true;
    }
    
    return false;
}

TriangleContainer::~TriangleContainer()
{
}
