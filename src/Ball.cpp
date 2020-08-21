#include "headers/Ball.h"

Ball::Ball() {
	r = 0.5f;
};

bool Ball::Update(Vector2 v, float size) {
	previousPosition = position;
	if (previousPosition.y == position.y && position.y >= 10.0f) {
		position.y -= 0.3f;
	}
	if (previousPosition.x >= position.x && position.x >= 15.0f) {
		position.x -= 0.3f;
	}
	if (previousPosition.x == position.x && position.x <= 0.0f) {
		position.x += 0.3f;
	}
	bool ret = false;
	hitPad = false;
	position.x += dx * GetFrameTime();
	position.y += dy * GetFrameTime();
	Vector2Normalize(position);

	if (position.x > 14.0f || position.x < 0.0f) {
		dx = -dx;
		ret = true;
	}
	if (position.y >= 10.0f) {
		dy = -dy;
	}
	
	if (position.x + r >= v.x - size / 2.0f && position.x - r <= v.x + size / 2.0f &&
		position.y <= v.y + r) {
		dx = (float)GetRandomValue(3, 6) * (position.x - v.x);
		dy = -(float)dy*GetRandomValue(4, 7);
		hitPad = true;

	}
	
	if (dy > 7.0f) dy = 7.0f;
	if (dx > 4.0f) dx = 4.0f;
	return ret;
}

void Ball::BrickCollision(int side) {
	switch (side) {
	case COLL_L: dx = -dx;
				 break;
	case COLL_U: dy = -dy;
				 dx = -dx;
				 break;
	}
}