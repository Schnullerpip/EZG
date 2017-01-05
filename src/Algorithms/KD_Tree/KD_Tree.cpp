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

KD_Tree::KD_Tree(unsigned dimension, std::vector<Shape*> shapes):k(dimension)
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

Node* Node::build(std::vector<TriangleContainer*> triangles, int depth, KD_Tree* kd_tree, BoundingBox* parent, bool left_child)
{
	kd_tree->incrSize();
	this->triangles = triangles;
	bbox = new BoundingBox();

	if (triangles.size() == 0) return this; //return an empty node

	bbox = triangles[0]->getBoundingBox(); //this is now needed anyway

	if (triangles.size() == 1) //return a node with the minimal props
	{
		left = new Node();
		right = new Node();
		left->triangles = std::vector<TriangleContainer*>();
		right->triangles = std::vector<TriangleContainer*>();
		return this;
	}

	//get a bounding box, that surrounds all the triangles
	for (auto t : triangles) {
		bbox->expand(t->getBoundingBox());
	}

	//now that the boundind box is complete - check if it should be trimmed, so it cant exceed the parents bounding box, or the median(split_point) of the parent
	if (parent)
	{
		if (bbox->FromX() < parent->FromX())bbox->setFromX(parent->FromX());
		if (bbox->FromY() < parent->FromY())bbox->setFromY(parent->FromY());
		if (bbox->FromZ() < parent->FromZ())bbox->setFromZ(parent->FromZ());

		if (bbox->ToX() > parent->ToX())bbox->setToX(parent->ToX());
		if (bbox->ToY() > parent->ToY())bbox->setToY(parent->ToY());
		if (bbox->ToZ() > parent->ToZ())bbox->setToZ(parent->ToZ());

		//check for exceeding the parent's median
		if (left_child && (bbox->*to_getters[parent->split_axis])() > parent->median) {
			(bbox->*to_setters[parent->split_axis])(parent->median);
		}
		else if(!left_child && (bbox->*from_getters[parent->split_axis])() < parent->median) {
			(bbox->*from_setters[parent->split_axis])(parent->median);
		}

		bbox->recalculate();
	}

	//the longest axis inside the boundig box? we will split on this axis!
	bbox->split_axis = bbox->longestAxis();


	//Get the right coordinate values for the split-axis respectively (if split on xAxis -> get all the x values)
	//- also count how many primitives are inside the bounding box
	std::set<float>* values = new std::set<float>();
	int prim_count = 0;

	Shape* last = nullptr;
	auto g = getters[bbox->split_axis];

	for (auto t : triangles) {
		if (last != t->getPrimitive()) { ++prim_count; last = t->getPrimitive(); }
		//values->insert(point_getters[bbox->split_axis](t->getMidPoint()));
		values->insert((t->*g)(t->A()));
		values->insert((t->*g)(t->B()));
		values->insert((t->*g)(t->C()));
	}
	//get the median of the x||y||z-coordinates
	std::vector<float>* vec_singles = new std::vector<float>(values->begin(), values->end());
	nth_element(vec_singles->begin(),  vec_singles->begin() + (vec_singles->size() / 2), vec_singles->end());
	bbox->median = (*vec_singles)[vec_singles->size() / 2];//this is the median element on the representive split axis
	float median = bbox->median;

	//TODO 
	//delete values;
	//delete vec_singles;

	std::vector<TriangleContainer*> triangles_left;
	std::vector<TriangleContainer*> triangles_right;

	int matches = 0;
	for (auto t : triangles)
	{
		bool isleft = false, isright = false;
		Cube* cu = (Cube*)t->getPrimitive();
		char* name = cu->name;
		//according to the split axis, divide the triangles representively (using the member function pointers PTMFs!)
		float a = (t->*g)(t->A()), b = (t->*g)(t->B()), c = (t->*g)(t->C());
		isright |= median < a;
		isright |= median < b;
		isright |= median < c;

		isleft |=  median > a;
		isleft |=  median > b;
		isleft |=  median > c;

		//if isleft and isright -> the triangle is split -> give it to both child nodes
		if (isleft && isright)
		{

			/*since the triangle is split the child's bounding box mus be adjusted,
			else overlapping bounding boxes are created*/
			//TODO

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

	//prepare the child-Nodes
	left = new Node();
	right = new Node();

	//now decide whether to keep on subdividing into further bounding boxes and nodes or to stop
	//if both left and right are almost the same triangles break the recursion
	if (triangles_left.size() >= 12 && triangles_right.size() >= 12)
	{
		//recurse down left and right
		left->build(triangles_left, depth + 1, kd_tree, bbox, true);
		right->build(triangles_right, depth + 1, kd_tree, bbox, false);


	}
	else //
	{
		//left->triangles = std::vector<TriangleContainer*>();
		//right->triangles = std::vector<TriangleContainer*>();
	}
	return this;
}

KD_Tree::~KD_Tree()
{
	delete root;
}
