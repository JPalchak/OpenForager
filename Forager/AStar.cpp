#include "AStar.h"
#include "PuzzleBoard.h"

#include <vector>
#include <list>

using namespace std;

/*

	SOLVER

*/
AStar::AStar()
{
}

AStar::~AStar()
{
}

Vector2* AStar::Solve(int* board, Vector2* toSolve)
{
	Vector2* move = nullptr;
	
	Vector2* end = FindViableGoal(toSolve);
	if (end == nullptr)
		return nullptr;

	Vector2* start = new Vector2();
	move = FindBestPathMove(start, end, board);

	//translate move
	if (move->x + 1 == start->x)
	{
		move->left = true;
	}
	else if (move->x - 1 == start->x)
	{
		move->x = start->x;
		move->left = false;
	}
	else if (move->y + 1 == start->y)
	{
		move->left = false;
	}
	else if (move->y - 1 == start->y)
	{
		move->y = start->y;
		move->left = true;
	}

	return move;
}

Vector2* AStar::FindViableGoal(Vector2* toSolve)
{
	return new Vector2(toSolve->x, BOARD_HEIGHT - 1);
}

Vector2* AStar::FindBestPathMove(Vector2* outStart, Vector2* end, int* board)
{
	int to = board[(BOARD_WIDTH * end->y) + end->x];
	int bestPathLength = 1000000;
	std::vector<Vector2*> bestPath;

	Vector2* start = new Vector2();
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT - 1; y++)
		{
			if (end->x == x && end->y == y)
					continue;

			int from = board[(BOARD_WIDTH * y) + x];

			//same piece?
			if (from == to)
			{
				if (PuzzleBoard::IsViableMove(board, x, y))
				{
					start->x = x;
					start->y = y;
					std::vector<Vector2*> path = GetPath(start, end, board);

					if (path.size() > 2)
					{
						if (path.size() < bestPathLength)
						{
							bestPathLength = path.size();

							while (!bestPath.empty())
							{
								delete bestPath.back();
								bestPath.pop_back();
							}

							bestPath = path;

							outStart->x = x;
							outStart->y = y;
						}
					}
				}
			}
		}
	}
	delete start;

	Vector2* bestMove = bestPath.back();
	while (!bestPath.empty())
	{
		if (bestPath.back() != bestMove)
			delete bestPath.back();

		bestPath.pop_back();
	}

	return bestMove;
}

std::vector<Vector2*> AStar::GetPath(Vector2* begin, Vector2* goal, int* board)
{
	vector<Vector2*> path;

	ANode* start = GetNodeFromBoard(board, new Vector2(begin));
	ANode* end = GetNodeFromBoard(board, new Vector2(goal));

	ANode* current = nullptr;
	ANode* child = nullptr;

	list<ANode*> open;
	list<ANode*> closed;
	list<ANode*>::iterator i;

	unsigned int n = 0;

	open.push_back(start);
	start->opened = true;

	while (n == 0 || (current != end && n < 50))
	{
		for (i = open.begin(); i != open.end(); ++i)
		{
			if (i == open.begin() || (*i)->GetScore() <= current->GetScore())
			{
				current = (*i);
			}
		}

		//we made it ma
		if (current == end)
			break;

		open.remove(current);
		current->opened = false;

		closed.push_back(current);
		current->closed = true;

		for (int x = -1; x < 2; x++)
		{
			for (int y = -1; y < 2; y++)
			{
				//no adjacents or current
				if ((x == 0 && y == 0) ||
					(x != 0 && y != 0))
					continue;

				child = GetNodeFromBoard(board, new Vector2(current->GetPosition()->x + x, current->GetPosition()->y + y));
				if (child == nullptr)
					continue;
				if (child->closed || !child->walkable)
					continue;

				if (child->opened)
				{
					if (child->GetGScore() > child->GetGScore(current))
					{
						child->SetParent(current);
						child->ComputeScore(end);
					}
				}
				else
				{
					open.push_back(child);
					child->opened = true;

					child->SetParent(current);
					child->ComputeScore(end);
				}
			}
		}

		n++;
	}

	while (current->HasParent() && current != start)
	{
		path.push_back(current->GetPosition());
		current = current->GetParent();
	}

	return path;
}

ANode* AStar::GetNodeFromBoard(int* board, Vector2* pos)
{
	if (pos->x < 0 || pos->x > BOARD_WIDTH - 1)
		return nullptr;
	if (pos->y < 0 || pos->y > BOARD_HEIGHT - 1)
		return nullptr;

	bool mobile = !PuzzleBoard::IsPieceImmobile(board[(BOARD_WIDTH * pos->y) + pos->x]);

	/*if (mobile)
	{
		mobile = (!PuzzleBoard::IsPieceImmobile(board[(BOARD_WIDTH * pos->y) + pos->x - 1]) &&
				  !PuzzleBoard::IsPieceImmobile(board[(BOARD_WIDTH * pos->y) + pos->x + 1]));
	}*/


	ANode* node = new ANode(pos, mobile);

	return node;
}