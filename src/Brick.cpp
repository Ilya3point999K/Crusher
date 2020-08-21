#include "headers/Brick.h"

Brick::Brick() {
	toughness = 0;
	type = 0;
	variant = GetRandomValue(0, 4);
	angle = (float)GetRandomValue(-45, 45);
	if (angle < 0) dir = true; else dir = false;
}

Brick::Brick(Vector2 pos) {
	position = pos;
	toughness = 1;
	type = 0;
	variant = GetRandomValue(0, 4);
	angle = (float)GetRandomValue(-45, 45);
	if (angle < 0) dir = true; else dir = false;
}

void Brick::Rotate(){
	if (dir) {
		angle += 10.0f * GetFrameTime();
		if (angle >= 45.0f) dir = false;
	} else {
		angle -= 10.0f * GetFrameTime();
		if (angle <= -45.0f) dir = true;
	}
}
