#include <stdio.h>
#include <tchar.h>

#include "WindowManager.h"

HWND WindowManager::window = nullptr;

WindowManager::WindowManager()
{
	//Normal
	this->puzzleColors[0][0][0] = 201;
	this->puzzleColors[0][0][1] = 178;
	this->puzzleColors[0][0][2] = 109;

	this->puzzleColors[0][1][0] = 46;
	this->puzzleColors[0][1][1] = 55;
	this->puzzleColors[0][1][2] = 54;

	this->puzzleColors[0][2][0] = 108;
	this->puzzleColors[0][2][1] = 101;
	this->puzzleColors[0][2][2] = 20;

	//CI
	this->puzzleColors[1][0][0] = 201;
	this->puzzleColors[1][0][1] = 178;
	this->puzzleColors[1][0][2] = 109;

	this->puzzleColors[1][1][0] = 46;
	this->puzzleColors[1][1][1] = 55;
	this->puzzleColors[1][1][2] = 54;

	this->puzzleColors[1][2][0] = 108;
	this->puzzleColors[1][2][1] = 101;
	this->puzzleColors[1][2][2] = 20;
}

void WindowManager::FindWindow()
{
	EnumWindows(SearchProc, NULL);

	if (WindowManager::window)
	{
		printf("YPP window found, searching for puzzle...\n\n");
	}
	else
	{
		Sleep(500);
	}
}

bool WindowManager::WindowFound()
{
	return WindowManager::window != nullptr;
}

bool WindowManager::WindowExists()
{
	return IsWindow(WindowManager::window);
}

bool WindowManager::WindowFocused()
{
	HWND curr = GetForegroundWindow();

	if (!curr)
		return false;

	if (!WindowManager::window)
		return false;

	return (GetWindowThreadProcessId(curr, NULL) == GetWindowThreadProcessId(WindowManager::window, NULL));
}

void WindowManager::ResetWindow()
{
	WindowManager::window = nullptr;
}

bool WindowManager::PuzzleFound(BYTE* image)
{
	if (image == nullptr)
		return false;


	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 3; i++)
		{
			if (GetPixelR(image, 204 + i, 21) != this->puzzleColors[j][i][0] || GetPixelG(image, 204 + i, 21) != this->puzzleColors[j][i][1] || GetPixelB(image, 204 + i, 21) != this->puzzleColors[j][i][2])
			{
				//we can delete the image here since from this point on it isn't used
				delete image;
				return false;
			}
		}
	}

	//we can NOT delete the image here because it is used in the logic loop
	return true;
}

Vector2* WindowManager::GetWindowPosition()
{
	RECT pos;
	GetWindowRect(WindowManager::window, &pos);

	Vector2* vec = new Vector2();
	vec->x = pos.left + 3;//possibly off a bit
	vec->y = pos.top + 25;

	return vec;
}

int WindowManager::GetPixelR(BYTE* data, int x, int y)
{
	return data[3 * ((y*SCREEN_WIDTH) + x) + 2];
}

int WindowManager::GetPixelG(BYTE* data, int x, int y)
{
	return data[3 * ((y*SCREEN_WIDTH) + x) + 1];
}

int WindowManager::GetPixelB(BYTE* data, int x, int y)
{
	return data[3 * ((y*SCREEN_WIDTH) + x)];
}

BYTE* WindowManager::GetImage()
{
	if (WindowManager::window)
	{

		HDC screen = GetDC(WindowManager::window);
		int sX = SCREEN_WIDTH;
		int sY = SCREEN_HEIGHT;

		BYTE* data = new BYTE[3 * sX * sY];

		HDC mem = CreateCompatibleDC(screen);
		HBITMAP bitmap = CreateCompatibleBitmap(screen, sX, sY);

		BITMAPINFOHEADER bmi = { 0 };
		bmi.biSize = sizeof(BITMAPINFOHEADER);
		bmi.biPlanes = 1;
		bmi.biBitCount = 24;
		bmi.biWidth = sX;
		bmi.biHeight = -sY;
		bmi.biCompression = BI_RGB;

		SelectObject(mem, bitmap);
		BitBlt(mem, 0, 0, sX, sY, screen, 0, 0, SRCCOPY);

		GetDIBits(screen, bitmap, 0, sY, data, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);

		DeleteObject(bitmap);
		DeleteDC(mem);
		ReleaseDC(WindowManager::window, screen);

		return data;
	}

	return nullptr;
}

BOOL CALLBACK WindowManager::SearchProc(HWND hWnd, LPARAM lParam)
{
	static TCHAR buffer[50];

	GetWindowText(hWnd, buffer, 50);

	if (_tcsstr(buffer, "Puzzle Pirates"))
	{
		WindowManager::window = hWnd;

		return FALSE;
	}

	return TRUE;
}