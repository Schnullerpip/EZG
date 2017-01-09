#include "KD_Tree.h"
#include <cassert>
#include <iostream>
#include <algorithm>
#include <set>
#include "Cube.h"

size_t KD_Tree::Size() const
{
	return size;
}

void KD_Tree::incrSize()
{
	++size;
}

Node* KD_Tree::Root() const
{
	return root;
}

size_t KD_Tree::getComplexityBound() const
{
	return complexits_bound;
}

void KD_Tree::setComplexityBound(size_t b)
{
	complexits_bound = b;
}

Shape* KD_Tree::fireRay(glm::vec3* origin, glm::vec3* direction, glm::vec3* out_collisionpoint, Node** out) const
{
	Ray* ray = new Ray(*origin, *direction);
	Shape* retVal = visitNodes(root, ray, out);
	if(ray->T())
		*out_collisionpoint = ray->beam(*ray->T());
	delete ray;
	return retVal;
}

Shape* KD_Tree::visitNodes(Node* node, Ray* ray, Node** out) const
{
	if (!node) return nullptr;

	if (node->bbox.hit(ray))
	{
		if (!node->left() || !node->right())
		{
			//inside Node that holds triangles, nobody else does
			//iterate through this nodes triangles and check for intersection
			float nearest_t = ray->getTMax()+1;
			Shape* nearest_prim = nullptr;
			for (auto t : node->triangles)
			{
				if (t->intersects(ray))
				{
					if (*ray->T() < nearest_t)
					{
						nearest_t = *ray->T();
						nearest_prim = t->getPrimitive();
					}
				}
			}
			if (node->bbox.surrounds(ray->beam(nearest_t)))
			{
				ray->setT(nearest_t);
				*out = node;
				return nearest_prim;
			}
		}

		//recurse down

		//which child to recurse first into?
		int first = ray->Origin()[node->bbox.split_axis] > node->bbox.median;

		Shape* retVal = visitNodes(node->children[first], ray, out);
		if (!retVal)
		{
			retVal = visitNodes(node->children[first^1], ray, out);
		}
		return retVal;
	}
	*out = nullptr;
	return nullptr;
}

KD_Tree::KD_Tree(unsigned dimension, std::vector<Shape*> shapes, size_t complexity):k(dimension), complexits_bound(complexity)
{
	const unsigned offset = 14; //the offset in the cube's vertice data (also includes color, normals etc.)
	root = new Node();

	//convert the shapes(primitives) to TriangleContainers
	std::vector<TriangleContainer*> triangles;
	//for each shape
	for (auto s : shapes)
	{
		GLfloat* triangle_start = &(*s->getVertices())[0];
		//for each shape's vertices
		for (int i = 0; i < s->getVertices()->size(); i+=offset*3)
		{
			triangles.push_back(new TriangleContainer(triangle_start+i, s, offset));
		}
	}
	//build the KD-Tree
	root->build(triangles, 0, this);
}

typedef float(TriangleContainer::*TC_Getter)(float*)const;
TC_Getter getters[] = {
	&TriangleContainer::getX,
	&TriangleContainer::getY,
	&TriangleContainer::getZ
};

float point_getX(Point3D& p) { return p.x; }
float point_getY(Point3D& p) { return p.y; }
float point_getZ(Point3D& p) { return p.z; }

float(*point_getters[])(Point3D& p) = {
	point_getX,
	point_getY,
	point_getZ
};

void (BoundingBox::*from_setters[])(float) = {
	&BoundingBox::setFromX,
	&BoundingBox::setFromY,
	&BoundingBox::setFromZ,
};
void (BoundingBox::*to_setters[])(float) = {
	&BoundingBox::setToX,
	&BoundingBox::setToY,
	&BoundingBox::setToZ,
};

typedef float (BoundingBox::*BB_Getter)()const;
BB_Getter from_getters[] = {
	&BoundingBox::FromX,
	&BoundingBox::FromY,
	&BoundingBox::FromZ,
};
BB_Getter to_getters[] = {
	&BoundingBox::ToX,
	&BoundingBox::ToY,
	&BoundingBox::ToZ,
};

//float(*3D_Getter[])(Point3D*) = {
//
//};

Node* Node::build(std::vector<TriangleContainer*> triangles, int depth, KD_Tree* kd_tree, Node* parent, bool left_child)
{
	kd_tree->incrSize();
	this->triangles = triangles;

	if (triangles.size() == 0) return this; //return an empty node

	bbox = triangles[0]->getBoundingBox(); //this is now needed anyway

	if (triangles.size() == 1) //return a node with the minimal props
	{
		left()->triangles = std::vector<TriangleContainer*>();
		right()->triangles = std::vector<TriangleContainer*>();
		return this;
	}

	//get a bounding box, that surrounds all the triangles
	for (auto t : triangles) {
		bbox.expand(t->getBoundingBox());
	}

	//now that the boundind box is complete - check if it should be trimmed, so it cant exceed the parents bounding box, or the median(split_point) of the parent
	if (parent)
	{
		if (bbox.FromX() < parent->bbox.FromX())bbox.setFromX(parent->bbox.FromX());
		if (bbox.FromY() < parent->bbox.FromY())bbox.setFromY(parent->bbox.FromY());
		if (bbox.FromZ() < parent->bbox.FromZ())bbox.setFromZ(parent->bbox.FromZ());

		if (bbox.ToX() > parent->bbox.ToX())bbox.setToX(parent->bbox.ToX());
		if (bbox.ToY() > parent->bbox.ToY())bbox.setToY(parent->bbox.ToY());
		if (bbox.ToZ() > parent->bbox.ToZ())bbox.setToZ(parent->bbox.ToZ());

		//check for exceeding the parent's median
		if (left_child && (bbox.*to_getters[parent->bbox.split_axis])() > parent->bbox.median) {
			(bbox.*to_setters[parent->bbox.split_axis])(parent->bbox.median);
		}
		else if(!left_child && (bbox.*from_getters[parent->bbox.split_axis])() < parent->bbox.median) {
			(bbox.*from_setters[parent->bbox.split_axis])(parent->bbox.median);
		}

		bbox.recalculate();
	}

	//the longest axis inside the boundig box? we will split on this axis!
	bbox.split_axis = bbox.longestAxis();


	//Get the right coordinate values for the split-axis respectively (if split on xAxis -> get all the x values)
	//- also count how many primitives are inside the bounding box
	std::set<float>* values = new std::set<float>();
	int prim_count = 0;

	Shape* last = nullptr;
	auto g = getters[bbox.split_axis];

	float bb_from = (bbox.*from_getters[bbox.split_axis])(), bb_to = (bbox.*to_getters[bbox.split_axis])();

	for (auto t : triangles) {
		if (last != t->getPrimitive()) { ++prim_count; last = t->getPrimitive(); }
		Point3D p = t->getMidPoint();
		float m = point_getters[bbox.split_axis](p), a, b, c;

		//if(m >= bb_from && m <= bb_to)
		//	values->insert(m);

		//a = (t->*g)(t->A());
		//b = (t->*g)(t->B());
		//c = (t->*g)(t->C());

		a = t->world_A[bbox.split_axis];
		b = t->world_B[bbox.split_axis];
		c = t->world_C[bbox.split_axis];

		if(a >= bb_from && a <= bb_to) values->insert(a);
		if(b >= bb_from && b <= bb_to) values->insert(b);
		if(c >= bb_from && c <= bb_to) values->insert(c);
	}
	if (values->empty())return this;
	//get the median of the x||y||z-coordinates
	std::vector<float>* vec_singles = new std::vector<float>(values->begin(), values->end());
	nth_element(vec_singles->begin(),  vec_singles->begin() + (vec_singles->size() / 2), vec_singles->end());
	bbox.median = (*vec_singles)[vec_singles->size() / 2];//this is the median element on the representive split axis
	float median = bbox.median;

	delete values;
	delete vec_singles;

	std::vector<TriangleContainer*> triangles_left;
	std::vector<TriangleContainer*> triangles_right;

	int matches = 0;
	for (auto t : triangles)
	{
		bool isleft = false, isright = false;
		Cube* cu = (Cube*)t->getPrimitive();
		char* name = cu->name;
		//according to the split axis, divide the triangles representively (using the member function pointers PTMFs!)
		//float a = (t->*g)(t->A()), b = (t->*g)(t->B()), c = (t->*g)(t->C());
		float a = t->world_A[bbox.split_axis], b = t->world_B[bbox.split_axis], c = t->world_C[bbox.split_axis];

		isright |= median < a;// && a <= bb_to;
		isright |= median < b;// && b <= bb_to;
		isright |= median < c;// && c <= bb_to;

		isleft |=  median > a;// && a >= bb_from;
		isleft |=  median > b;// && b >= bb_from;
		isleft |=  median > c;// && c >= bb_from;

		//if isleft and isright -> the triangle is split -> give it to both child nodes
		if (isleft && isright)
		{
			//both underlying bounding boxes need to be able to 'hit' the triangle, so push it to both of them
			triangles_left.push_back(t);
			triangles_right.push_back(t);
			++matches;
		}
		else if (!isleft) //its completely on the right of the splitting point -> give it to the right child node
		{
			triangles_right.push_back(t);
		}
		else //its completely on the left of the splitting point -> give it to the left child node
		{
			triangles_left.push_back(t);
		}
	} 

	//now decide whether to keep on subdividing into further bounding boxes and nodes or to stop
	//if only one single primitive participates in the bounding box and it is not too complex, we dont need to subdivide any further
	if (triangles.size() <= kd_tree->getComplexityBound() || triangles_left.size() == matches || triangles_right.size() == matches) return this;

	//prepare the child-Nodes
	children[0] = new Node(this);
	children[1] = new Node(this);
	left()->build(triangles_left, depth + 1, kd_tree, this, true);
	right()->build(triangles_right, depth + 1, kd_tree, this, false);
	return this;
}

KD_Tree::~KD_Tree()
{
	delete root;
}

Node* Node::left() const
{
	return children[0];
}

Node* Node::right() const
{
	return children[1];
}
