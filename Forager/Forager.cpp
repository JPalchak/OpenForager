#include <stdio.h>
#include <thread>
#include <ctime>

#include "Forager.h"
#include "PuzzleBoard.h"

Forager::Forager()
{
	this->astar = new AStar();
	this->tree = new GameTree();
}

Forager::~Forager()
{
	delete this->astar;
}

std::vector<Vector2*> Forager::Solve(int* board)
{
	std::vector<Vector2*> moves;

	printf("Solving...\n");

	if (!PuzzleBoard::CratesExist(board))
	{
		printf("     No crates, searching...\n");
		moves.push_back(FindChests(board));
	}
	else
	{
		printf("Starting tree solve...\n");
		Vector2* move = tree->SimulateMoves(board, 1);

		if (move == nullptr)
		{
			Vector2* toSolve = PuzzleBoard::GetBestChest(board);
			
			printf("No move was found, solving via A*\n\n");

			move = astar->Solve(board, toSolve);
			delete toSolve;

			moves.push_back(move);
		}
		else
		{
			if (move->left)
			{
				printf("     left @ (%d, %d)\n", move->x, move->y);
			}
			else
			{
				printf("     right @ (%d, %d)\n", move->x, move->y);
			}

			moves.push_back(move);
		}
	}

	return moves;
}

Vector2* Forager::FindChests(int* board)
{
	int* simBoard = new int[(BOARD_WIDTH * BOARD_HEIGHT)];

	for (int x = 0; x < BOARD_WIDTH - 1; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT - 1; y++)
		{
			int breaks = 0;
			PuzzleBoard::CopyBoards(board, simBoard);
			//printf("     Boards copied...\n");
			
			if (PuzzleBoard::IsViableMove(simBoard, x, y))
			{
				//printf("     Simulating move left...\n");
				PuzzleBoard::SimulateMove(simBoard, x, y, true);

				//printf("     Simulating board changes...\n");

				int weight = 0;
				breaks = PuzzleBoard::SimulateBreak(simBoard, nullptr, weight);
				if (weight >= 3)
				{
					//printf("          Good move found!\n");
					Vector2* move = new Vector2();
					move->x = x;
					move->y = y;
					move->left = true;

					printf("left move: (%d, %d)\n", x, y);

					delete[] simBoard;

					return move;
				}

				//printf("     Recopying board...\n");
				PuzzleBoard::CopyBoards(board, simBoard);
			}

			if (PuzzleBoard::IsViableMove(simBoard, x, y))
			{
				//printf("     Simulating move right...\n");

				PuzzleBoard::SimulateMove(simBoard, x, y, false);

				//printf("     Simulating board changes...\n");

				int weight = 0;
				breaks = PuzzleBoard::SimulateBreak(simBoard, nullptr, weight);
				if (weight >= 3)
				{
					//printf("          Good move found!\n");

					Vector2* move = new Vector2();
					move->x = x;
					move->y = y;
					move->left = false;

					printf("right move: (%d, %d)\n", x, y);

					delete[] simBoard;

					return move;
				}
			}

			//printf("No viable moves found\n");
		}
	}

	delete[] simBoard;

	return nullptr;
}