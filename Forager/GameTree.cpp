#include "GameTree.h"
#include "PuzzleBoard.h"

GameTree::GameTree()
{
}

GameTree::~GameTree()
{
	delete[] this->solveThreads;

	while (!nodes.empty())
	{
		delete nodes.back();
		nodes.pop_back();
	}
}

Vector2* GameTree::SimulateMoves(int* board, int maxDepth)
{
	Vector2* toSolve = PuzzleBoard::GetBestChest(board);

	if (toSolve == nullptr)
		return nullptr;

	//printf("Solving (%d, %d)\n", toSolve->x, toSolve->y);

	int depth = 0;

	int startX = toSolve->x - 3;
	if (startX < 0)
		startX = 0;

	int endX = toSolve->x + 3;
	if (endX > BOARD_WIDTH - 1)
		endX = BOARD_WIDTH + 1;

	int* simBoard = new int[BOARD_WIDTH * BOARD_HEIGHT];

	//simulate all possible initial moves
	for (int x = startX; x < endX; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT - 1; y++)
		{
			if (PuzzleBoard::IsViableMove(board, x, y))
			{
				//we get the weight they represent, and the position they represent is (X, Y)
				int weight = PuzzleBoard::TryBothMoves(x, y, true, board, simBoard, toSolve);
				nodes.push_back(new GameNode(simBoard, weight, 0, new Vector2(x, y, true)));

				weight = PuzzleBoard::TryBothMoves(x, y, false, board, simBoard, toSolve);
				nodes.push_back(new GameNode(simBoard, weight, 0, new Vector2(x, y, false)));
			}
		}
	}

	printf("%d initial nodes created, solving...\n", nodes.size());

	clock_t start = clock();

	//find best move
	/*int simplestMoveCount = 100;
	int moveCount;

	Vector2* move = new Vector2();

	for (unsigned int i = 0; i < nodes.size(); i++)
	{
		moveCount = 999;

		GameNode* node = nodes[i];
		node->SimulateMove(depth, maxDepth, toSolve);

		moveCount = node->GetTreeSolvedDepth();

		if (moveCount != 999)
		{
			if (moveCount < simplestMoveCount)
			{
				printf("Got better move @ I-Node[%d](%d, %d) with depth %d\n", i, node->GetMove()->x, node->GetMove()->y, moveCount);

				move->x = node->GetMove()->x;
				move->y = node->GetMove()->y;
				move->left = node->GetMove()->left;

				if (moveCount == 0)
					break;

				simplestMoveCount = moveCount;
				//1 = 3... for maxDepth
				//2 = 3... for moveCount
				maxDepth = moveCount - 1;
			}
		}

		delete node;
	}*/

	Vector2* move = new Vector2();
	
	int moveCounts[TREE_THREADS];
	GameNode* solvedNodes[TREE_THREADS];

	int simplestMoveCount = 100;

	for (unsigned int i = 0; i < nodes.size(); i += TREE_THREADS)
	{
		for (int j = 0; j < TREE_THREADS; j++)
		{
			if (i + j >= nodes.size())
				break;

			GameNode* n = nodes.at(i + j);
			solvedNodes[j] = n;

			this->solveThreads[j] = std::async(&GameTree::ThreadedSolveNode, n, toSolve, maxDepth);
		}

		for (int j = 0; j < TREE_THREADS; j++)
		{
			if (i + j >= nodes.size())
				break;

			moveCounts[j] = this->solveThreads[j].get();
		}

		printf("Joined %d threads @ %d\n", TREE_THREADS, i / TREE_THREADS);

		for (int j = 0; j < TREE_THREADS; j++)
		{
			if (i + j >= nodes.size())
				break;

			if (moveCounts[j] != 999)
			{
				if (moveCounts[j] < simplestMoveCount)
				{
					move->x = solvedNodes[j]->GetMove()->x;
					move->y = solvedNodes[j]->GetMove()->y;
					move->left = solvedNodes[j]->GetMove()->left;

					if (moveCounts[j] == 0)
						goto complete;

					simplestMoveCount = moveCounts[j];
					maxDepth = moveCounts[j] - 1;
				}
			}

			delete solvedNodes[j];
			solvedNodes[j] = nullptr;
		}
	}

	complete:

	nodes.clear();
	delete toSolve;

	clock_t end = clock();

	float time = float(end - start) / CLOCKS_PER_SEC;
	printf("time: %f\n", time);

	if (move->x == -1)
		return nullptr;

	return move;
}

int GameTree::ThreadedSolveNode(GameNode* node, Vector2* toSolve, int maxDepth)
{
	int simDepth = 0;

	node->SimulateMove(simDepth, maxDepth, toSolve);

	return node->GetTreeSolvedDepth();
}