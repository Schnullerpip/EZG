#pragma once
#include "Shape.h"
#include "BoundingBox.h"
#include "TriangleContainer.h"

struct Node;
struct NodeX;
struct NodeY;
struct NodeZ;

/**
 * A Datastructute splitting in three K Dimensions
 */
class KD_Tree
{
	unsigned k;
	size_t size = 0;
	Node* root;
	size_t complexits_bound;//default is cube complexity
	Shape* visitNodes(Node*, Ray*, Node** out = nullptr) const;
public:
	size_t Size()const;
	void incrSize();
	Node* Root()const;
	size_t getComplexityBound()const;
	void setComplexityBound(size_t b);

	Shape* fireRay(glm::vec3* origin, glm::vec3* direction, glm::vec3* out_collisionpoint, Node** out) const;

	KD_Tree(unsigned dimension, std::vector<Shape*> shapes, size_t complexity = 12);//default is cube complexity
	KD_Tree(unsigned dimension, std::vector<TriangleContainer*> tris, size_t complexity = 32);
	~KD_Tree();
};

/**
 * the base node implementing basic behaviour and leaving specific parts to detailled implementations -> strategy pattern
 */
struct Node
{
	BoundingBox bbox;
	Node* children[2] = {nullptr, nullptr};
	Node* left()const;
	Node* right()const;

	Node* parent;
	std::vector<TriangleContainer*> triangles;
	int depth;

	Node() :parent(nullptr), depth(0) {}
	Node(Node* parent):parent(parent), depth(parent->depth+1){}

	Node* build(std::vector<TriangleContainer*> triangles, int depth, KD_Tree* kd_tree, Node* parent = nullptr, bool left_child = false);

	virtual ~Node()
	{
		if(left())
			delete left();
		if(right())
			delete right();
	}
};