#pragma once
#include <raylib.h>
#include <math.h>

class Brick
{
public:
	Vector2 position;
	char type, toughness, variant;
	Brick();
	Brick(Vector2 pos);
	void Rotate();
	float angle;
	bool dir;
};

