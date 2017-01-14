#include "TriangleContainer.h"

//udeful helper functions
float getx(const glm::vec3* triangle) { return triangle->x; }
float gety(const glm::vec3* triangle) { return triangle->y; }
float getz(const glm::vec3* triangle) { return triangle->z; }
bool greatest(float a, float b) { return a > b; }
bool smallest(float a, float b) { return !greatest(a, b); }

TriangleContainer::TriangleContainer(float* triangleLocation, Shape* prim, int offset):primitive(prim), triangle(triangleLocation), offset(offset)
{
	/*get translatet rotatet global coordinate values ---------------------*/
	glm::vec3 coordinatesA = glm::vec3(getXLocal(A()), getYLocal(A()), getZLocal(A()));
	glm::vec3 coordinatesB = glm::vec3(getXLocal(B()), getYLocal(B()), getZLocal(B()));
	glm::vec3 coordinatesC = glm::vec3(getXLocal(C()), getYLocal(C()), getZLocal(C()));

	glm::mat4 model_A;
	glm::mat4 model_B;
	glm::mat4 model_C;

	model_A = glm::translate(model_A, primitive->getPosition());
	model_B = glm::translate(model_B, primitive->getPosition());
	model_C = glm::translate(model_C, primitive->getPosition());

	model_A = glm::rotate(model_A, getPrimitive()->rotation_angle, getPrimitive()->rotation_axis);
	model_B = glm::rotate(model_B, getPrimitive()->rotation_angle, getPrimitive()->rotation_axis);
	model_C = glm::rotate(model_C, getPrimitive()->rotation_angle, getPrimitive()->rotation_axis);

	world_A = model_A * glm::vec4(coordinatesA, 1) ;
	world_B = model_B * glm::vec4(coordinatesB, 1);
	world_C = model_C * glm::vec4(coordinatesC, 1);
	/*---------------------------------------------------------------------*/
}
TriangleContainer::TriangleContainer(glm::vec3 A, glm::vec3 B ,glm::vec3 C, Shape* primitive): primitive(primitive), triangle(nullptr), offset(0), world_A(A), world_B(B), world_C(C)
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

float TriangleContainer::getXLocal(float* triangle) const
{
	return *triangle;
}

float TriangleContainer::getYLocal(float* triangle) const
{
	return *(triangle + 1);
}

float TriangleContainer::getZLocal(float* triangle) const
{
	return *(triangle + 2);
}

bool TriangleContainer::equals(TriangleContainer* tc)const
{
	 return (A() == tc->A() && B() == tc->B() && C() == tc->C());
}


float TriangleContainer::most(float(*genericGetter)(const glm::vec3*), bool (*generic_comparision)(float a, float b), float offset) const
{
		float target = genericGetter(&world_A) + offset, tmp;
		if (generic_comparision(tmp = genericGetter(&world_B) + offset, target)) { target = tmp; }
		if (generic_comparision(tmp = genericGetter(&world_C) + offset, target)) { target = tmp; }

		return target;
	}

/** returns a bounding box after processing the actual bounds*/
BoundingBox TriangleContainer::getBoundingBox() const
{
	float lowest_x = most(getx, smallest);
	float highest_x = most(getx, greatest);

	float lowest_y = most(gety, smallest);
	float highest_y = most(gety, greatest);

	float lowest_z = most(getz, smallest);
	float highest_z = most(getz, greatest);

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
	float mid_x = world_A.x + world_B.x + world_C.x;
	float mid_y = world_A.y + world_B.y + world_C.y;
	float mid_z = world_A.z + world_B.z + world_C.z;
	return Point3D(mid_x/3, mid_y/3, mid_z/3);
}

glm::vec3 toVec(const TriangleContainer* tr, float* t)
{
	return glm::vec3(tr->getX(t), tr->getY(t), tr->getZ(t));
}

#define EPSILON 0.000001
bool TriangleContainer::intersects(Ray *ray) const
{
	glm::vec3 edge1 = world_B - world_A;
    glm::vec3 edge2 = world_C - world_A;
    
    glm::vec3 q = glm::cross(ray->Direction(), edge2);
    float det = glm::dot(edge1, q);
    
    if(det > -EPSILON && det < EPSILON)
        return false;
    
    float inv_det = 1.0f/det;
    
    glm::vec3 s = ray->Origin() - world_A;
    
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
