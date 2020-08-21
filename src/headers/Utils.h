#pragma once
#include <raylib.h>
#include <rlgl.h>
#include <external/par_shapes.h>
#include <stdlib.h>

#define SMALL 0
#define MEDIUM 1
#define LARGE 2

class Utils
{
public:
	Utils();
	Mesh GenRock();
	Mesh GenWall();
	Mesh GenBall();
	Mesh GenGem(int size);
	Mesh GenRocket();
	Mesh GenDynamite();
	Color RequestGemColor();
	Texture numbers[10];
	void DrawPlaneTexture(Texture texture, Vector3 centerPos, Vector2 size, Color color);
	void DrawScore(unsigned int score, float x, float y);
	void ShakeCamera(Camera &camera);
};

