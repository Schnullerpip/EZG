#include "KD_Tree.h"

KD_Tree::KD_Tree(unsigned dimension): k(dimension)
{
}


KD_Tree::~KD_Tree()
{
}

bool NodeX::smaller(Node* n)
{
	return data.x < n->data.x;
}
bool NodeY::smaller(Node* n)
{
	return data.y < n->data.y;
}
bool NodeZ::smaller(Node* n)
{
	return data.z < n->data.z;
}

