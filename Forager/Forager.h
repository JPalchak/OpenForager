#pragma once

#ifndef _FORAGER_H
#define _FORAGER_H

#include <Windows.h>

#include "GameTree.h"
#include "AStar.h"
#include "Vector2.h"

class Forager
{
public:
	Forager();
	~Forager();

	std::vector<Vector2*> Solve(int* board);

private:
	Vector2* FindChests(int* board);

private:
	AStar* astar;
	GameTree* tree;
};

#endif