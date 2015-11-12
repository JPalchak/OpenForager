#pragma once

#ifndef _A_STAR_H
#define _A_STAR_H

#include "Vector2.h"
#include "ANode.h"

#include <vector>

class AStar
{
public:
	AStar();
	~AStar();

	Vector2* Solve(int* board, Vector2* toSolve);

private:
	Vector2* FindBestPathMove(Vector2* start, Vector2* end, int* board);
	Vector2* FindViableGoal(Vector2* toSolve);
	std::vector<Vector2*> GetPath(Vector2* start, Vector2* goal, int* board);

	ANode* GetNodeFromBoard(int* board, Vector2* pos);
};

#endif