#pragma once

#ifndef _GAME_NODE_H
#define _GAME_NODE_H

#include "Vector2.h"

#include <vector>

class GameNode
{
public:
	GameNode(int* board, int weight, int depth, Vector2* pos);
	~GameNode();
	
	void SimulateMove(int depth, int maxDepth, Vector2* toSolve);

	int GetTreeSolvedDepth();

	Vector2* GetMove();

	//BFS
	void AddChildrenToVector(std::vector<GameNode*>* queue);

	int GetDepth();
	int GetWeight();

private:
	int ourDepth;
	int ourWeight;

	Vector2* ourMove;

	int* ourBoard;

	std::vector<GameNode*> children;
};

#endif