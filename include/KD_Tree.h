#pragma once

struct Node;
struct NodeX;
struct NodeY;
struct NodeZ;
struct Point;

class KD_Tree
{
	unsigned k;
	Node* root;
public:
	KD_Tree(unsigned dimension);
	~KD_Tree();
};

struct Point
{
	float x, y, z;
};

struct Node
{
protected:
	~Node();
public:
	Point data;
	Node* left;
	Node* right;
	virtual bool smaller(Node*) = 0;
};

struct NodeX : public Node
{
	bool smaller(Node*) override;
	~NodeX();
};
struct NodeY : public Node
{
	bool smaller(Node*) override;
	~NodeY();
};
struct NodeZ : public Node
{
	bool smaller(Node*) override;
	~NodeZ();
};