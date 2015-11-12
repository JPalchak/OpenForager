#pragma once

#ifndef _A_NODE_H
#define _A_NODE_H

#include "Vector2.h"

class ANode
{
public:
	ANode();
	ANode(Vector2* pos, bool w);
	~ANode();

	Vector2* GetPosition();

	ANode* GetParent();
	void SetParent(ANode* parent);
	bool HasParent();

	int GetScore();
	void ComputeScore(ANode* end);

	int GetGScore(ANode* node);
	int GetHScore(ANode* node);
	int GetGScore();
	int GetHScore();

public:
	bool closed;
	bool opened;
	bool walkable;

private:
	ANode* parent;

	Vector2* pos;

	int f, g, h;
};

#endif