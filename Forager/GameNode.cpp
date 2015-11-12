#include "GameNode.h"
#include "PuzzleBoard.h"

GameNode::GameNode(int* board, int weight, int depth, Vector2* pos)
{
	this->ourBoard = new int[BOARD_WIDTH * BOARD_HEIGHT];
	PuzzleBoard::CopyBoards(board, this->ourBoard);

	this->ourWeight = weight;
	this->ourMove = pos;

	this->ourDepth = depth;
}

GameNode::~GameNode()
{
	delete [] this->ourBoard;
	delete this->ourMove;

	while (!children.empty())
	{
		GameNode* node = children.back();
		children.pop_back();
		delete node;
	}
}

void GameNode::SimulateMove(int depth, int maxDepth, Vector2* toSolve)
{
	int startX = this->ourMove->x - 3;
	if (startX < 0)
		startX = 0;

	int endX = this->ourMove->x + 3;
	if (endX > BOARD_WIDTH - 1)
		endX = BOARD_WIDTH - 1;

	int* simBoard = new int[BOARD_WIDTH * BOARD_HEIGHT];

	for (int x = startX; x < endX; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT - 1; y++)
		{
			if (PuzzleBoard::IsViableMove(this->ourBoard, x, y))
			{
				int weight = PuzzleBoard::TryBothMoves(x, y, true, this->ourBoard, simBoard, toSolve);
				children.push_back(new GameNode(simBoard, weight, this->ourDepth + 1, new Vector2(x, y, true)));

				weight = PuzzleBoard::TryBothMoves(x, y, false, this->ourBoard, simBoard, toSolve);
				children.push_back(new GameNode(simBoard, weight, this->ourDepth + 1, new Vector2(x, y, false)));
			}
		}
	}

	depth++;
	if (depth > maxDepth)
		return;

	//propogate
	for (int i = 0; i < children.size(); i++)
	{
		this->children[i]->SimulateMove(depth, maxDepth, toSolve);
	}
}

/*
NEW PROBLEM:
	When this finds a solution, it assumes it is the ONLY soltuion in the tree
		and that there is no other...
	I.E. This is depth first, not breadth first


*/
int GameNode::GetTreeSolvedDepth()
{
	std::vector<GameNode*> queue;
	queue.push_back(this);
	//queue.reserve(10240);

	int bestDepth = 999;

	GameNode* node;
	for (int i = 0; i < queue.size(); i++)
	{
		node = queue.at(i);

		if (node->GetWeight() == CHEST_WAS_CLEARED)
		{
			if (node->GetDepth() < bestDepth)
			{
				bestDepth = node->GetDepth();
			}
		}

		if (node->GetDepth() < bestDepth)
		{
			node->AddChildrenToVector(&queue);
		}
	}

	return bestDepth;
}

Vector2* GameNode::GetMove()
{
	return this->ourMove;
}

int GameNode::GetDepth()
{
	return this->ourDepth;
}

int GameNode::GetWeight()
{
	return this->ourWeight;
}

void GameNode::AddChildrenToVector(std::vector<GameNode*>* queue)
{
	for (int i = 0; i < this->children.size(); i++)
	{
		queue->push_back(children.at(i));
	}
}