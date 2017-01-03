#include "KD_Tree.h"
#include <cassert>
#include <iostream>
#include <algorithm>

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
	//iterate through all the shapes and add their vertices to the kd tree - TODO ideally always first searching for the median to achieve a balanced tree
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

Node* Node::build(std::vector<TriangleContainer*> triangles, int depth, KD_Tree* kd_tree)
{
	kd_tree->incrSize();
	triangles = triangles;
	bbox = BoundingBox();

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

	//get a bounding box, that surrounds all the triangles - also count how many primitives are still in the queue
	Shape* last = nullptr;
	int prim_count = 0;
	for (auto t : triangles) {
		bbox.expand(t->getBoundingBox());
		if (last != t->getPrimitive())
		{
			++prim_count;
			last = t->getPrimitive();
		}
	}

	//the longest axis inside the boundig box? we will split on this axis!
	int split_axis = bbox.longestAxis();


	//Get the right coordinate values for the split-axis respectively (if split on xAxis -> get all the x values)
	std::vector<float> values;
	auto g = getters[split_axis];
	for (auto t : triangles) {
		values.push_back((t->*g)(t->A()));
		values.push_back((t->*g)(t->B()));
		values.push_back((t->*g)(t->C()));
	}
	//get the median of the x||y||z-coordinates
	std::nth_element(values.begin(),  values.begin() + (values.size() / 2), values.end());
	float median = values[values.size() / 2];//this is the median element on the representive split axis

	std::vector<TriangleContainer*> triangles_left;
	std::vector<TriangleContainer*> triangles_right;

	for (auto t : triangles)
	{
		bool isleft = false, isright = false;
		//according to the split axis, divide the triangles representively (using the member function pointers PTMFs!)
		float a = (t->*g)(t->A()), b = (t->*g)(t->B()), c = (t->*g)(t->C());
		isright |= split_axis < a;
		isright |= split_axis < b;
		isright |= split_axis < c;

		isleft |= split_axis > a;
		isleft |= split_axis > b;
		isleft |= split_axis > c;

		//if isleft and isright -> the triangle is split -> give it to both the child nodes
		if (isleft && isright)
		{
			triangles_left.push_back(t);
			triangles_right.push_back(t);
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
	int matches = 0;
	for (auto t : triangles_left)
	{
		for (auto o : triangles_right)
		{
			if (t->equals(o))
			{
				++matches;
				continue;
			}
		}
	}

	float bla = (float)matches / triangles_left.size();
	if ((float)matches / triangles_left.size() < 0.5 && (float)matches / triangles_right.size() < 0.5)
	{
		//recurse down left and right
		left = left->build(triangles_left, depth + 1, kd_tree);
		right = left->build(triangles_right, depth + 1, kd_tree);
	}
	else
	{
		left->triangles = std::vector<TriangleContainer*>();
		right->triangles = std::vector<TriangleContainer*>();
	}

	return this;
}

KD_Tree::~KD_Tree()
{
	delete root;
}
