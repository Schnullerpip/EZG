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
	Shape* visitNodes(Node*, Ray*) const;
public:
	size_t Size()const;
	void incrSize();
	Node* Root()const;

	Shape* fireRay(glm::vec3* origin, glm::vec3* direction, glm::vec3* out_collisionpoint) const;

	KD_Tree(unsigned dimension, std::vector<Shape*> shapes);
	~KD_Tree();
};

/**
 * the base node implementing basic behaviour and leaving specific parts to detailled implementations -> strategy pattern
 */
struct Node
{
	BoundingBox* bbox = nullptr;
	Node* left = nullptr;
	Node* right = nullptr;
	std::vector<TriangleContainer*> triangles;

	Node(){}

	Node* build(std::vector<TriangleContainer*> triangles, int depth, KD_Tree* kd_tree, BoundingBox* parent = nullptr, bool left_child = false);

	virtual ~Node()
	{
		delete bbox;
		if(left)
			delete left;
		if(right)
			delete right;
	}
};

/**
 * A concrete Node, seperating by the x Axis and generating NodeYs
 */
struct NodeX : public Node
{
	NodeX() {}
	//bool smaller(Point) override;
	//Node* generateChildNode(Point p)override;
};
/**
 * A concrete Node, seperating by the y Axis and generating NodeZs
 */
struct NodeY : public Node
{
	NodeY() {}
	//bool smaller(Point) override;
	//Node* generateChildNode(Point p)override;
};
/**
 * A concrete Node, seperating by the z Axis and generating NodeXs
 */
struct NodeZ : public Node
{
	NodeZ(){}
	//bool smaller(Point) override;
	//Node* generateChildNode(Point p)override;
};