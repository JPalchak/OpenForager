#pragma once

#ifndef _GAME_TREE_H
#define _GAME_TREE_H

#include <thread>
#include <future>

#include "GameNode.h"

#define TREE_THREADS 7

class GameTree
{
public:
	GameTree();
	~GameTree();

	Vector2* SimulateMoves(int* board, int maxDepth);

public:
	Vector2* desired;

private:
	static int ThreadedSolveNode(GameNode* node, Vector2* toSolve, int maxDepth);

private:
	std::vector<GameNode*> nodes;

	std::future<int> solveThreads[TREE_THREADS];
};

#endif