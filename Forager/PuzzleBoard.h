#include <Windows.h>
#include "Vector2.h"

#define BOARD_WIDTH 7
#define BOARD_HEIGHT 10

#define BOARD_X 67
#define BOARD_Y 50

#define PIECE_OFFSET 23
#define PIECE_SIZE 45

#define CHEST_WAS_CLEARED -1337

namespace PuzzleBoard
{
	enum BLOCK
	{
		//system
		BROKEN = 777,
		CRATE_IMMOBILE = 888,
		UNKNOWN = 999,
		UNKNOWN_PIECE = 666,

		//singles
		DIRT = 0,
		LEAF = 1,
		SAND = 2,
		STONE = 3,
		CLOTH = 4,

		//1x1 crates
		BANANA_CRATE1 = 5,
		COCONUT_CRATE1 = 6,
		LIME_CRATE1 = 7,
		MANGO_CRATE1 = 8,
		PINEAPPLE_CRATE1 = 9,
		EMPTY_CRATE1 = 10,
		CI_CRATE1 = 11,

		//2x2 crates
		BOTTOM_L1_CRATE2 = 12,
		BOTTOM_R1_CRATE2 = 13,
		BOTTOM_L2_CRATE2 = 14,
		BOTTOM_R2_CRATE2 = 15,
		BOTTOM_L3_CRATE2 = 16,
		BOTTOM_R3_CRATE2 = 17,
		BOTTOM_L4_CRATE2 = 18,
		BOTTOM_R4_CRATE2 = 19,
		BOTTOM_L5_CRATE2 = 20,
		BOTTOM_R5_CRATE2 = 21,
		BOTTOM_L6_CRATE2 = 22,
		BOTTOM_R6_CRATE2 = 23,
		BOTTOM_LCI_CRATE2 = 24,
		BOTTOM_RCI_CRATE2 = 25,

		//3x2 crates
		BOTTOM_LCI_CRATE3 = 26,
		BOTTOM_MCI_CRATE3 = 27,
		BOTTOM_RCI_CRATE3 = 28,

		//special pieces
		MACHETE = 29,
		SHOVEL = 30,
		MONKEY = 31,
		QUAKE = 32
	};

	bool SimulateBreak(int* board, Vector2* desired, int& weight);
	void ReadBoard(BYTE* image, int* board);
	void PrintBoard(int* board);
	
	void FindBreaks(int* board);
	void ShiftPieces(int* board, int& weight);

	void SimulateMove(int* board, int x, int y, bool left);

	char GetPieceCharacter(int piece);

	int CountBreaks(int* board, Vector2* desired, int& weight);
	
	Vector2* MonkiesExist(int* board);
	Vector2* ClearLocalSpecials(int* board, Vector2* toSolve);

	bool CrateDropped(int* board, Vector2* desired);
	bool CratesExist(int* board);
	bool IsCrate1(int piece);
	bool IsCrate2L(int piece);
	bool IsCrate2R(int piece);
	bool IsCrate3(int piece);
	bool IsPieceImmobile(int piece);
	bool IsViableMove(int* board, int x, int y);

	int TryBothMoves(int x, int y, bool left, int* sourceBoard, int* simBoard, Vector2* desired);

	Vector2* GetBestChest(int* board);

	inline bool ContainsUnknown(int* board)
	{
		for (int x = 0; x < BOARD_WIDTH; x++)
		{
			for (int y = 0; y < BOARD_HEIGHT; y++)
			{
				if (board[(BOARD_WIDTH * y) + x] == BLOCK::UNKNOWN)
					return true;
			}
		}

		return false;
	}

	inline void CopyBoards(int* src, int * dest)
	{
		for (int x = 0; x < BOARD_WIDTH; x++)
		{
			for (int y = 0; y < BOARD_HEIGHT; y++)
			{
				dest[(BOARD_WIDTH * y) + x] = src[(BOARD_WIDTH * y) + x];
			}
		}
	}

	inline int FindPieceFromRGB(int r, int g, int b)
	{
		if (r == 93 && g == 66 && b == 56)
			return BLOCK::DIRT;
		else if (r == 15 && g == 50 && b == 11)
			return BLOCK::LEAF;
		else if (r == 225 && g == 203 && b == 136)
			return BLOCK::SAND;
		else if (r == 92 && g == 83 && b == 93)
			return BLOCK::STONE;
		else if (r == 139 && g == 123 && b == 91)
			return BLOCK::CLOTH;
		else if (r == 253 && g == 252 && b == 87)
			return BLOCK::BANANA_CRATE1;
		else if (r == 183 && g == 101 && b == 65)
			return BLOCK::COCONUT_CRATE1;
		else if (r == 201 && g == 243 && b == 33)
			return BLOCK::LIME_CRATE1;
		else if (r == 254 && g == 189 && b == 30)
			return BLOCK::MANGO_CRATE1;
		else if (r == 254 && g == 158 && b == 23)
			return BLOCK::PINEAPPLE_CRATE1;
		else if (r == 120 && g == 89 && b == 63)
			return BLOCK::EMPTY_CRATE1;
		else if (r == 95 && g == 71 && b == 47)
			return BLOCK::CI_CRATE1;
		else if (r == 146 && g == 73 && b == 35)
			return BLOCK::BOTTOM_L1_CRATE2;
		else if (r == 233 && g == 210 && b == 166)
			return BLOCK::BOTTOM_R1_CRATE2;
		else if (r == 190 && g == 182 && b == 144)
			return BLOCK::BOTTOM_L2_CRATE2;
		else if (r == 119 && g == 88 && b == 67)
			return BLOCK::BOTTOM_R2_CRATE2;
		else if (r == 175 && g == 112 && b == 64)
			return BLOCK::BOTTOM_L3_CRATE2;
		else if (r == 237 && g == 225 && b == 175)
			return BLOCK::BOTTOM_R3_CRATE2;
		else if (r == 179 && g == 110 && b == 60)
			return BLOCK::BOTTOM_L4_CRATE2;
		else if (r == 223 && g == 206 && b == 145)
			return BLOCK::BOTTOM_R4_CRATE2;
		else if (r == 204 && g == 199 && b == 174)
			return BLOCK::BOTTOM_L5_CRATE2;
		else if (r == 89 && g == 68 && b == 42)
			return BLOCK::BOTTOM_R5_CRATE2;
		else if (r == 255 && g == 255 && b == 253)
			return BLOCK::BOTTOM_L6_CRATE2;
		else if (r == 188 && g == 180 && b == 169)
			return BLOCK::BOTTOM_R6_CRATE2;
		else if (r == 255 && g == 215 && b == 125)
			return BLOCK::BOTTOM_LCI_CRATE2;
		else if (r == 188 && g == 180 && b == 174)
			return BLOCK::BOTTOM_RCI_CRATE2;
		else if (r == 136 && g == 120 && b == 85)
			return BLOCK::SHOVEL;
		else if (r == 205 && g == 217 && b == 245)
			return BLOCK::MACHETE;
		else if (r == 152 && g == 128 && b == 100)
			return BLOCK::MONKEY;
		else if (r == 59 && g == 64 && b == 10)
			return BLOCK::QUAKE;
		else
			return BLOCK::UNKNOWN;
	}

	inline bool BreaksExist(int* board)
	{
		for (int x = 0; x < BOARD_WIDTH; x++)
		{
			for (int y = 0; y < BOARD_HEIGHT; y++)
			{
				if (board[(BOARD_WIDTH * y) + x] == BLOCK::BROKEN)
					return true;
			}
		}

		return false;
	}
};