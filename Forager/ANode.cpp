#include "ANode.h"

#include <math.h>

/*

NODE

*/
ANode::ANode()
{
	this->parent = nullptr;

	this->closed = false;
	this->opened = false;

	f = g = h = 0;

	this->pos = new Vector2();
}

ANode::ANode(Vector2* position, bool w)
{
	this->parent = nullptr;

	this->closed = false;
	this->opened = false;

	f = g = h = 0;

	this->walkable = w;
	this->pos = position;
}

ANode::~ANode()
{
	if (this->pos != nullptr)
		delete this->pos;
}

Vector2* ANode::GetPosition()
{
	return this->pos;
}

ANode* ANode::GetParent()
{
	return this->parent;
}

void ANode::SetParent(ANode* par)
{
	this->parent = par;
}

bool ANode::HasParent()
{
	return (this->parent != nullptr);
}

int ANode::GetScore()
{
	return f;
}

void ANode::ComputeScore(ANode* end)
{
	g = GetGScore(this->parent);
	h = GetHScore(end);

	f = g + h;
}

int ANode::GetGScore(ANode* n)
{
	if (n == nullptr)
		return 0;

	return n->g + ((this->pos->x == n->pos->x || this->pos->y == n->pos->y) ? 10 : 14);
}

int ANode::GetGScore()
{
	return g;
}

int ANode::GetHScore(ANode* n)
{
	if (n == nullptr)
		return 0;

	return (abs(n->pos->x - this->pos->x) + abs(n->pos->y - this->pos->y)) * 10;
}

int ANode::GetHScore()
{
	return h;
}