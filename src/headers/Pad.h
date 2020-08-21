#pragma once
#include <raylib.h>

class Pad
{
public:
	Vector2 position;
	BoundingBox box;
	float size;
	Pad();
	float& getX() { return position.x; };
	float& getZ() { return position.y; };
};

