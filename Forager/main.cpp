#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#include "PuzzleBoard.h"
#include "WindowManager.h"
#include "MouseManager.h"
#include "Forager.h"

static bool isMouseMoving = false;
POINT lastMouse;

int main()
{
	//is auto?
	bool autoMove = true;
	GetCursorPos(&lastMouse);

	//object
	WindowManager* window = new WindowManager();
	Forager bot = Forager();
	MouseManager mouse = MouseManager(0);

	//non-object
	int* board = new int[BOARD_WIDTH * BOARD_HEIGHT];


	//----------------------------------------------------------------------


	printf("Welcome to Forager by Pixelgriffin\n\n");

searchPoint:
	//search for the YPP window
	printf("Searching for YPP window..,\n");
	while (!window->WindowFound())
	{
		window->FindWindow();
	}

	//at this point we have found the window
	while (window->WindowExists())
	{
		while (window->WindowFocused())
		{
			//create image
			BYTE* image = window->GetImage();

			while (window->PuzzleFound(image))
			{
				//read board in
				PuzzleBoard::ReadBoard(image, board);

				//solve puzzle
				std::vector<Vector2*> moves = bot.Solve(board);
				Vector2* move = nullptr;
				//if (move != nullptr)
				//	delete move;

				if (autoMove)
				{
					while (!moves.empty())
					{
						move = moves.back();
						moves.pop_back();

						if (move != nullptr)
						{
							POINT currentMouse;
							GetCursorPos(&currentMouse);

							if (currentMouse.x == lastMouse.x && currentMouse.y == lastMouse.y)
							{
								printf("trying to move\n");

								//make moves
								Vector2* pos = window->GetWindowPosition();
								pos->x += BOARD_X + (move->x * 45) + 22;
								pos->y += BOARD_Y + (move->y * 45) + 22;

								mouse.ExecuteMovementTo(window, *pos, 25);

								if (move->left)
									mouse.ExecuteLeftClick();
								else
									mouse.ExecuteRightClick();
							}

							GetCursorPos(&lastMouse);

							delete move;

							Sleep(100 + MouseManager::RandomRange(0, 500));
						}
					}
				}

				delete [] image;
				image = window->GetImage();
			}
		}
	}

	window->ResetWindow();
	goto searchPoint;

	return 0;
}