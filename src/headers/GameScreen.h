#pragma once
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <vector>
#include "Brick.h"
#include "Pad.h"
#include "Ball.h"
#include "Utils.h"

#include "../libs/rlights.h"

#define SHATTER 0
#define SPARK 1
#define SHARD 2
#define FLARE 3

#define BURST 10
#define SPLASH 11
#define EXPLOSION 12
#define TORCH 13

class GameScreen
{
	static const int WIDTH = 15;
	static const int HEIGHT = 10;
	static const int MAX_PARTICLES = 3000;
public:
	void Restart();
	GameScreen();
	std::vector<Brick> bricks;
	Pad pad;
	Ball ball;
	float rockSpeed;
	unsigned int score;
	char explosiveCooldown;
	void Render();
	bool Update(bool pause);

private:
	enum ENCOUNTERS { NOTHING, AVALANCHE, SHIFT };

	Sound hit1, hit2, rolling, ingot, glass1, glass2, jewelry, avalanche, shift, pickaxe1, pickaxe2,
		wall1, wall2, rocket_start, explosion, wood;
	Music ambience;

	Utils utils;
	Camera camera;

	Model rockModels[5];
	Model ingotModel;
	Model smallGem;
	Model mediumGem;
	Model largeGem;

	Model wallModels[3];

	Model ballModel;
	Model padModel;
	Model torchModel;
	Model rocketModel;
	Model dynamiteModel;

	typedef struct {
		Vector3 position;
		float direction;
		bool isAlive;
		bool explode;
		float r;
		float speed;
		float lightTime;
	} Explosive;
	Explosive rocket;

	void Explosion(Vector2 center, float r);

	Shader shader;
	Color lightColors[3];
	Light lights[MAX_LIGHTS] = { 0 };
	bool flash; float flashx, flashy;

	typedef struct {
		char type, variant;
		Vector3 position;
		Vector3 direction;
		float lifeTime;
		bool isAlive;
	} Particle;
	std::vector<Particle> particles;

	void RenderParticles();
	void UpdateParticles();
	void RequestShatters(float x, float y, float dx, float dy);
	void RequestSparks(float x, float y, float z, float dx, float dy, char form);
	void RequestShards(float x, float y, float dx, float dy);
	//void RequestFlares(float x, float y, float z, float dx, float dy);

	void MoveModels();
	void GenNewBricks();
	void RandomEncounter(int type);

	bool gameOver;
	bool start;
	bool stopGen;

	float timer;
	char stopGenTimer;
	char shakeTimer;

};

