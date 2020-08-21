#pragma once
#include <raylib.h>
#include <raymath.h>
#include "Brick.h"

#define COLL_L 0
#define COLL_U 1
#define COLL_R 2
#define COLL_D 3

class Ball
{
public:
	Vector2 position;
	Vector2 previousPosition;
	float dx, dy;
	float r;
	bool hitPad;
	Ball();
	float& getX() { return position.x; };
	float& getZ() { return position.y; };
	bool Update(Vector2 v, float size);
	void BrickCollision(int side);
};

