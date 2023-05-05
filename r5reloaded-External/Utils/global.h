#pragma once
#include "Memory\Memory.h"

struct global
{
	uint64_t BaseAddress = 0;
	RECT GameSize = {};
	POINT GamePoint = {};

	// System
	bool Active = false;
	bool VSync = true;

	// Visual
	bool ESP = true;
};

struct MATRIX
{
	float matrix[16];
};

extern global g;
extern MATRIX;