#pragma once

#ifndef _WINDOW_MANAGER_H
#define _WINDOW_MANAGER_H

#include <Windows.h>

#include "Vector2.h"

#define SCREEN_WIDTH 800//450
#define SCREEN_HEIGHT 600//600

class WindowManager
{
public:
	WindowManager();

	void UpdateImage();
	BYTE* GetImage();

	void FindWindow();
	void ResetWindow();

	bool WindowFocused();
	bool WindowFound();
	bool WindowExists();
	bool PuzzleFound(BYTE* image);

	static int GetPixelR(BYTE* data, int x, int y);
	static int GetPixelG(BYTE* data, int x, int y);
	static int GetPixelB(BYTE* data, int x, int y);

	Vector2* GetWindowPosition();

private:
	int puzzleColors[2][3][3];
	BYTE* imageData;

	static HWND window;

	static BOOL CALLBACK SearchProc(HWND, LPARAM);
};

#endif