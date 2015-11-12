#include <stdio.h>

#include "WindowManager.h"
#include "PuzzleBoard.h"

using namespace PuzzleBoard;

bool PuzzleBoard::SimulateBreak(int* board, Vector2* desired, int& weight)
{
	bool clearedCrate = false;

	FindBreaks(board);
	CountBreaks(board, desired, weight);

	int i = 0;
	while (true)
	{
		ShiftPieces(board, weight);

		if (CrateDropped(board, desired))
			clearedCrate = true;

		FindBreaks(board);
		CountBreaks(board, desired, weight);

		if (!BreaksExist(board))
			break;

		i++;
	}

	//printf("(breaks found: %d)\n", count);

	return clearedCrate;
}

void PuzzleBoard::ReadBoard(BYTE* image, int* board)
{
	//posibly just clear the board rather than reallocate
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			board[(BOARD_WIDTH * y) + x] = BLOCK::UNKNOWN;
		}
	}

	//read
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			//if (board[(BOARD_WIDTH * y) + x] == BLOCK::CRATE_IMMOBILE)
			//	continue;

			board[(BOARD_WIDTH * y) + x] = FindPieceFromRGB(
				WindowManager::GetPixelR(image, BOARD_X + (x * PIECE_SIZE) + PIECE_OFFSET, BOARD_Y + (y * PIECE_SIZE) + PIECE_OFFSET),  //r
				WindowManager::GetPixelG(image, BOARD_X + (x * PIECE_SIZE) + PIECE_OFFSET, BOARD_Y + (y * PIECE_SIZE) + PIECE_OFFSET),  //g
				WindowManager::GetPixelB(image, BOARD_X + (x * PIECE_SIZE) + PIECE_OFFSET, BOARD_Y + (y * PIECE_SIZE) + PIECE_OFFSET)); //b

			/*if (IsCrate2(board[(BOARD_WIDTH * y) + x]))
			{
				board[(BOARD_WIDTH * (y - 1)) +	x] = BLOCK::CRATE_IMMOBILE;
				board[(BOARD_WIDTH * (y - 1)) + (x + 1)] = BLOCK::CRATE_IMMOBILE;
			}
			else if (IsCrate3(board[(BOARD_WIDTH * y) + x]))
			{
				board[(BOARD_WIDTH * (y - 1)) + x] = BLOCK::CRATE_IMMOBILE;
				board[(BOARD_WIDTH * (y - 1)) + (x + 1)] = BLOCK::CRATE_IMMOBILE;
				board[(BOARD_WIDTH * (y - 1)) + (x + 2)] = BLOCK::CRATE_IMMOBILE;
			}*/
		}
	}
}

Vector2* PuzzleBoard::MonkiesExist(int* board)
{
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			if (board[(BOARD_WIDTH * y) + x] == BLOCK::MONKEY)
			{
				Vector2* move = new Vector2();
				move->x = x;
				move->y = y;

				return move;
			}
		}
	}

	return nullptr;
}

void PuzzleBoard::FindBreaks(int* board)
{
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			int piece = board[(BOARD_WIDTH * y) + x];

			if (piece == BLOCK::UNKNOWN || piece == BLOCK::BROKEN)
				continue;

			int i = 1;
			while ((i < (7 - x)) && (piece == board[(BOARD_WIDTH * y) + (x + i)]))
				i++;

			if (i >= 3)
			{
				for (int r = 0; r < i; r++)
					board[(BOARD_WIDTH * y) + (x + r)] = BLOCK::BROKEN;
			}

			i = 1;
			while ((i < (10 - y)) && (piece == board[(BOARD_WIDTH * (y + i)) + x]))
				i++;

			if (i >= 3)
			{
				for (int r = 0; r < i; r++)
					board[(BOARD_WIDTH * (y + r)) + x] = BLOCK::BROKEN;
			}
		}
	}
}

int PuzzleBoard::CountBreaks(int* board, Vector2* desired, int& weight)
{
	int count = 0;

	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			int piece = board[(BOARD_WIDTH * y) + x];

			if (piece == BLOCK::BROKEN)
			{
				count++;
				if (desired == nullptr)
					weight++;

				int piece2;
				for (int i = y; i >= 0; i--)
				{
					if (desired == nullptr)
					{
						piece2 = board[(BOARD_WIDTH * i) + x];

						if (IsCrate1(piece2) || IsCrate2L(piece2) || IsCrate2R(piece2))
							weight += 100;
					}
					else if(i == desired->y)
					{
						if (x == desired->x)
						{
							weight += 100;
						}
						else if (!desired->single)
						{
							bool isLast = (board[(BOARD_WIDTH * 6) + x] == BLOCK::UNKNOWN);
							/*bool isLast = true;

							if (!IsCrate2L(board[(BOARD_WIDTH * 7) + x]) &&
								!IsCrate2R(board[(BOARD_WIDTH * 7) + x]))
							{
								for (int j = 6; j <= 0; j--)
								{
									if (IsCrate2L(board[(BOARD_WIDTH * j) + x]) ||
										IsCrate2R(board[(BOARD_WIDTH * j) + x]))
										break;

									if (board[(BOARD_WIDTH * j) + x] != BLOCK::UNKNOWN)
									{
										isLast = false;
										break;
									}
								}
							}*/

							if (isLast)
							{
								if (desired->left && (x == desired->x + 1))
								{
									weight -= 100;
								}
								else if (!desired->left && (x == desired->x - 1))
								{
									weight -= 100;
								}
							}
						}
					}
				}
			}
		}
	}

	return count;
}

int PuzzleBoard::TryBothMoves(int x, int y, bool left, int* sourceBoard, int* simBoard, Vector2* desired)
{
	PuzzleBoard::CopyBoards(sourceBoard, simBoard);


	if (left)
	{
		if (PuzzleBoard::IsViableMove(simBoard, x, y))
		{
			PuzzleBoard::SimulateMove(simBoard, x, y, true);

			int weight = 0;
			if (PuzzleBoard::SimulateBreak(simBoard, desired, weight))
				return CHEST_WAS_CLEARED;

			return weight;
		}
	}
	else
	{
		if (PuzzleBoard::IsViableMove(simBoard, x, y))
		{
			PuzzleBoard::SimulateMove(simBoard, x, y, false);

			int weight = 0;
			if (PuzzleBoard::SimulateBreak(simBoard, desired, weight))
				return CHEST_WAS_CLEARED;

			return weight;
		}
	}

	return 0;
}

void PuzzleBoard::ShiftPieces(int* board, int& weight)
{
	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = 0; y < BOARD_HEIGHT; y++)
		{
			if (board[(BOARD_WIDTH * y) + x] == BLOCK::BROKEN)
			{

				for (int i = y; i > 0; i--)
				{
					if (IsCrate2L(board[(BOARD_WIDTH * y) + x]))
					{
						if (board[(BOARD_WIDTH * (i + 1)) + (x + 1)] != BLOCK::BROKEN &&
							board[(BOARD_WIDTH * (i + 1)) + (x + 1)] != BLOCK::UNKNOWN)
						{
							board[(BOARD_WIDTH * (i + 1)) + x] = BLOCK::UNKNOWN;
							break;
						}
					}

					if (IsCrate2R(board[(BOARD_WIDTH * y) + x]))
					{
						if (board[(BOARD_WIDTH * (i + 1)) + (x - 1)] != BLOCK::BROKEN &&
							board[(BOARD_WIDTH * (i + 1)) + (x - 1)] != BLOCK::UNKNOWN)
						{
							board[(BOARD_WIDTH * (i + 1)) + x] = BLOCK::UNKNOWN;
							break;
						}
					}

					board[(BOARD_WIDTH * i) + x] = board[(BOARD_WIDTH * (i - 1)) + x];
				}

				board[x] = BLOCK::UNKNOWN;
			}
		}
	}
}

bool PuzzleBoard::IsCrate2R(int piece)
{
	return (piece == BLOCK::BOTTOM_R1_CRATE2 ||
		piece == BLOCK::BOTTOM_R2_CRATE2 ||
		piece == BLOCK::BOTTOM_R3_CRATE2 ||
		piece == BLOCK::BOTTOM_R4_CRATE2 ||
		piece == BLOCK::BOTTOM_R5_CRATE2 ||
		piece == BLOCK::BOTTOM_R6_CRATE2 ||
		piece == BLOCK::BOTTOM_RCI_CRATE2 ||
		piece == BLOCK::BOTTOM_MCI_CRATE3 ||
		piece == BLOCK::BOTTOM_RCI_CRATE3);
}

bool PuzzleBoard::IsCrate2L(int piece)
{
	return (piece == BLOCK::BOTTOM_L1_CRATE2 ||
		piece == BLOCK::BOTTOM_L2_CRATE2 ||
		piece == BLOCK::BOTTOM_L3_CRATE2 ||
		piece == BLOCK::BOTTOM_L4_CRATE2 ||
		piece == BLOCK::BOTTOM_L5_CRATE2 ||
		piece == BLOCK::BOTTOM_L6_CRATE2 ||
		piece == BLOCK::BOTTOM_LCI_CRATE2);
}

bool PuzzleBoard::IsCrate1(int piece)
{
	return (piece == BLOCK::BANANA_CRATE1 ||
		piece == BLOCK::CI_CRATE1 ||
		piece == BLOCK::COCONUT_CRATE1 ||
		piece == BLOCK::EMPTY_CRATE1 ||
		piece == BLOCK::LIME_CRATE1 ||
		piece == BLOCK::MANGO_CRATE1 ||
		piece == BLOCK::PINEAPPLE_CRATE1);
}

bool PuzzleBoard::IsCrate3(int piece)
{
	return (piece == BLOCK::BOTTOM_LCI_CRATE3);
}

bool PuzzleBoard::IsPieceImmobile(int piece)
{
	return (piece == BLOCK::BOTTOM_R1_CRATE2 ||
		piece == BLOCK::BOTTOM_R2_CRATE2 ||
		piece == BLOCK::BOTTOM_R3_CRATE2 ||
		piece == BLOCK::BOTTOM_R4_CRATE2 ||
		piece == BLOCK::BOTTOM_R5_CRATE2 ||
		piece == BLOCK::BOTTOM_R6_CRATE2 ||
		piece == BLOCK::BOTTOM_RCI_CRATE2 ||
		piece == BLOCK::BOTTOM_MCI_CRATE3 ||
		piece == BLOCK::BOTTOM_RCI_CRATE3 ||
		piece == BLOCK::BOTTOM_L1_CRATE2 ||
		piece == BLOCK::BOTTOM_L2_CRATE2 ||
		piece == BLOCK::BOTTOM_L3_CRATE2 ||
		piece == BLOCK::BOTTOM_L4_CRATE2 ||
		piece == BLOCK::BOTTOM_L5_CRATE2 ||
		piece == BLOCK::BOTTOM_L6_CRATE2 ||
		piece == BLOCK::BOTTOM_LCI_CRATE2 ||
		piece == BLOCK::CRATE_IMMOBILE ||
		piece == BLOCK::BANANA_CRATE1 ||
		piece == BLOCK::COCONUT_CRATE1 ||
		piece == BLOCK::LIME_CRATE1 || 
		piece == BLOCK::MANGO_CRATE1 ||
		piece == BLOCK::PINEAPPLE_CRATE1 ||
		piece == BLOCK::EMPTY_CRATE1 ||
		piece == BLOCK::CI_CRATE1 || 
		piece == BLOCK::MONKEY ||
		piece == BLOCK::QUAKE ||
		piece == BLOCK::SHOVEL ||
		piece == BLOCK::MACHETE ||
		piece == BLOCK::UNKNOWN);
}

bool PuzzleBoard::IsViableMove(int* board, int x, int y)
{
	if (board[(BOARD_WIDTH * y) + x] == BLOCK::SHOVEL)
		return true;
	if (board[(BOARD_WIDTH * y) + x] == BLOCK::MACHETE)
		return true;
	if (board[(BOARD_WIDTH * y) + x] == BLOCK::MONKEY)
		return true;
	if (board[(BOARD_WIDTH * y) + x] == BLOCK::QUAKE)
		return true;

	//if all the pieces are the same, no simulation is needed
	if (board[(BOARD_WIDTH * y) + x] == board[(BOARD_WIDTH * y) + (x + 1)] &&
		board[(BOARD_WIDTH * y) + x] == board[(BOARD_WIDTH * (y + 1)) + (x + 1)] &&
		board[(BOARD_WIDTH * y) + x] == board[(BOARD_WIDTH * (y + 1)) + x])
		return false;

	if (IsPieceImmobile(board[(BOARD_WIDTH * y) + x]) ||
		IsPieceImmobile(board[(BOARD_WIDTH * y) + (x + 1)]) ||
		IsPieceImmobile(board[(BOARD_WIDTH * (y + 1)) + x]) ||
		IsPieceImmobile(board[(BOARD_WIDTH * (y + 1)) + (x + 1)]))
		return false;

	if (board[(BOARD_WIDTH * y) + (x + 1)] == BLOCK::SHOVEL ||
		board[(BOARD_WIDTH * (y + 1)) + (x + 1)] == BLOCK::SHOVEL ||
		board[(BOARD_WIDTH * (y + 1)) + x] == BLOCK::SHOVEL)
		return false;

	if (board[(BOARD_WIDTH * y) + (x + 1)] == BLOCK::MACHETE ||
		board[(BOARD_WIDTH * (y + 1)) + (x + 1)] == BLOCK::MACHETE ||
		board[(BOARD_WIDTH * (y + 1)) + x] == BLOCK::MACHETE)
		return false;

	if (board[(BOARD_WIDTH * y) + (x + 1)] == BLOCK::QUAKE ||
		board[(BOARD_WIDTH * (y + 1)) + (x + 1)] == BLOCK::QUAKE ||
		board[(BOARD_WIDTH * (y + 1)) + x] == BLOCK::QUAKE)
		return false;

	if (board[(BOARD_WIDTH * y) + (x + 1)] == BLOCK::MONKEY ||
		board[(BOARD_WIDTH * (y + 1)) + (x + 1)] == BLOCK::MONKEY ||
		board[(BOARD_WIDTH * (y + 1)) + x] == BLOCK::MONKEY)
		return false;

	return true;
}

bool PuzzleBoard::CrateDropped(int* board, Vector2* desired)
{
	if (desired == nullptr)
		return false;

	int piece = board[(BOARD_WIDTH * 9) + desired->x];

	if (IsCrate1(piece) || IsCrate2L(piece) || IsCrate2R(piece))
		return true;

	/*for (int y = desired->y + 1; y < BOARD_HEIGHT; y++)
	{
		int piece2 = board[(BOARD_WIDTH * y) + desired->x];
		if (piece2 != BLOCK::UNKNOWN)
		{
			//printf("NOOOOOOOOOO @ (%d, %d)", desired->x, y);
			return false;
		}
	}

	printf("dropped!");*/

	return false;
}

void PuzzleBoard::SimulateMove(int* board, int x, int y, bool left)
{
	int piece = board[(BOARD_WIDTH * y) + x];
	if (piece == BLOCK::SHOVEL)
	{
		for (int i = y; i < BOARD_HEIGHT; i++)
		{
			int piece2 = board[(BOARD_WIDTH * i) + x];

			if (!IsCrate1(piece2) && !IsCrate2L(piece2) && !IsCrate2R(piece2))
				board[(BOARD_WIDTH * i) + x] = BLOCK::BROKEN;
		}

		return;
	}

	if (piece == BLOCK::MACHETE)
	{
		if (left)
		{
			for (int i = 0; i <= x; i++)
			{
				int piece2 = board[(BOARD_WIDTH * y) + i];

				if (!IsCrate1(piece2) && !IsCrate2L(piece2) && !IsCrate2R(piece2))
					board[(BOARD_WIDTH * y) + i] = BLOCK::BROKEN;
			}
		}
		else
		{
			//printf("Executing MACHETE (%d)\n", y);

			for (int i = x; i <= BOARD_WIDTH; i++)
			{
				int piece2 = board[(BOARD_WIDTH * y) + i];

				if (!piece2 == BLOCK::UNKNOWN && !IsCrate1(piece2) && !IsCrate2L(piece2) && !IsCrate2R(piece2))
					board[(BOARD_WIDTH * y) + i] = BLOCK::BROKEN;
			}
		}

		return;
	}

	if (y == BOARD_HEIGHT)
		return;

	int temp[4];

	temp[0] = board[(BOARD_WIDTH * y) + x];
	temp[1] = board[(BOARD_WIDTH * y) + (x + 1)];
	temp[2] = board[(BOARD_WIDTH * (y + 1)) + (x + 1)];
	temp[3] = board[(BOARD_WIDTH * (y + 1)) + x];

	if (left)
	{
		board[(BOARD_WIDTH * y) + x] = temp[1];
		board[(BOARD_WIDTH * y) + (x + 1)] = temp[2];
		board[(BOARD_WIDTH * (y + 1)) + (x + 1)] = temp[3];
		board[(BOARD_WIDTH * (y + 1)) + x] = temp[0];
	}
	else
	{
		board[(BOARD_WIDTH * y) + x] = temp[3];
		board[(BOARD_WIDTH * y) + (x + 1)] = temp[0];
		board[(BOARD_WIDTH * (y + 1)) + (x + 1)] = temp[1];
		board[(BOARD_WIDTH * (y + 1)) + x] = temp[2];
	}
}

void PuzzleBoard::PrintBoard(int* board)
{
	for (int y = 0; y < BOARD_HEIGHT; y++)
	{
		for (int x = 0; x < BOARD_WIDTH; x++)
		{
			char c;

			c = GetPieceCharacter(board[(BOARD_WIDTH * y) + x]);

			printf("%c", c);
		}

		printf("\n");
	}

	printf("\n\n");
}

char PuzzleBoard::GetPieceCharacter(int piece)
{
	if (piece == BLOCK::CLOTH)
		return 'c';
	else if (piece == BLOCK::DIRT)
		return 'd';
	else if (piece == BLOCK::LEAF)
		return 'l';
	else if (piece == BLOCK::SAND)
		return 's';
	else if (piece == BLOCK::STONE)
		return 'r';
	else if (IsCrate2L (piece) || IsCrate2R(piece) || IsCrate3(piece) || IsCrate1(piece) || piece == BLOCK::CRATE_IMMOBILE)
		return '*';

	return '?';
}

bool PuzzleBoard::CratesExist(int* board)
{
	for (int y = 0; y < BOARD_HEIGHT; y++)
	{
		for (int x = 0; x < BOARD_WIDTH; x++)
		{
			if (IsCrate2L(board[(BOARD_WIDTH * y) + x]) ||
				IsCrate2R(board[(BOARD_WIDTH * y) + x]) ||
				IsCrate3(board[(BOARD_WIDTH * y) + x]) ||
				IsCrate1(board[(BOARD_WIDTH * y) + x]))
				return true;
		}
	}

	return false;
}

Vector2* PuzzleBoard::ClearLocalSpecials(int* board, Vector2* toSolve)
{
	for (int y = 0; y < BOARD_HEIGHT; y++)
	{
		int piece = board[(BOARD_WIDTH * y) + toSolve->x];

		if (piece == BLOCK::SHOVEL)
		{
			Vector2* move = new Vector2();
			move->y = y;
			move->x = toSolve->x;
			move->left = true;

			return move;
		}
		else if (piece == BLOCK::MACHETE || piece == BLOCK::QUAKE || piece == BLOCK::MONKEY)
		{
			if (y > toSolve->y)
			{
				Vector2* move = new Vector2();
				move->y = y;
				move->x = toSolve->x;
				move->left = true;

				return move;
			}
		}
	}

	return nullptr;
}

Vector2* PuzzleBoard::GetBestChest(int* board)
{
	for (int y = (BOARD_HEIGHT - 1); y >= 0; y--)
	{
		for (int x = 0; x < BOARD_WIDTH - 1; x++)
		{
			int piece = board[(BOARD_WIDTH * y) + x];

			if (PuzzleBoard::IsCrate2L(piece))
			{
				Vector2* move = new Vector2();

				if (x < 3)
				{
					move->left = true;
					move->single = false;
					move->y = y;

					bool completed = true;
					for (int i = (y + 1); i < BOARD_HEIGHT; i++)
					{
						if (board[(BOARD_WIDTH * i) + x] != PuzzleBoard::BLOCK::UNKNOWN)
						{
							completed = false;
							break;
						}
					}

					if (completed)
					{
						move->x = x + 1;
						move->left = false;
					}
					else
						move->x = x;
				}
				else
				{
					move->left = false;
					move->single = false;
					move->y = y;

					bool completed = true;
					for (int i = (y + 1); i < BOARD_HEIGHT; i++)
					{
						if (board[(BOARD_WIDTH * i) + (x + 1)] != PuzzleBoard::BLOCK::UNKNOWN)
						{
							completed = false;
							break;
						}
					}

					if (completed)
					{
						move->x = x;
						move->left = true;
					}
					else
						move->x = x + 1;
				}

				return move;
			}
		}
	}

	for (int x = 0; x < BOARD_WIDTH; x++)
	{
		for (int y = BOARD_HEIGHT; y >= 0; y--)
		{
			int piece = board[(BOARD_WIDTH * y) + x];

			if (PuzzleBoard::IsCrate1(piece))
			{
				Vector2* chest = new Vector2();
				chest->left = false;
				chest->x = x;
				chest->y = y;
				chest->single = true;

				return chest;
			}
		}
	}

	return nullptr;
}