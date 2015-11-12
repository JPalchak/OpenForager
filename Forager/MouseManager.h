#pragma once

#ifndef _MOUSE_MANAGER_H
#define _MOUSE_MANAGER_H

#include "WindowManager.h"
#include "Vector2.h"

class MouseManager
{
public:
	MouseManager(int pointThreshold);

	void ExecuteMovementTo(WindowManager* wm, Vector2 to, int threshold);
	void ExecuteLeftClick();
	void ExecuteRightClick();

	static int RandomRange(int min, int max);
	static double NextDouble();

private:
	int GetPoint(int n1, int n2, float perc);

private:
	int thresh;
};

#endif