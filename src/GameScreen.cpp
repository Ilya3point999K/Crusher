#define RLIGHTS_IMPLEMENTATION
#include "headers/GameScreen.h"

void GameScreen::Restart() {
	//TraceLog(LOG_INFO, FormatText("%i", bricks.size()));
	bricks.clear();
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			if (y >= 5) {
				if (GetRandomValue(1, 100) % 5 == 0) {
					Brick b({ (float)x, (float)y });
					b.variant = GetRandomValue(0, 4);
					bricks.push_back(b);
				}
			}
		}
	}
	//TraceLog(LOG_INFO, FormatText("%i", bricks.size()));
	for (int i = 0; i < WIDTH * HEIGHT - bricks.size(); i++) {
		Brick b;
		bricks.push_back(b);
	}
	//TraceLog(LOG_INFO, FormatText("%i", bricks.size()));
	particles.clear();
	for (int i = 0; i < MAX_PARTICLES; i++) {
		Particle p = { 0 };
		p.isAlive = false;
		particles.push_back(p);
	}

	ball.getX() = pad.position.x;
	ball.getZ() = pad.position.y + ball.r;

	rocket = { 0 };
	rocket.r = 2.0f;
	rocket.speed = 1.0f;
	rocket.isAlive = false;

	start = false;
	gameOver = false;
	flash = false;
	shakeTimer = 0;
	timer = 0;
	rockSpeed = 0.2f;
	explosiveCooldown = 10;
	score = 0;
}

GameScreen::GameScreen() {
	camera = { 0 };
	camera.fovy = 90.0f;
	camera.position = { 7.0f, 10.0f, 0.0f };
	camera.target = { 7.0f, 0.0f, 0.0f };
	camera.up = { 0.0f, 0.0f, 1.0f };
	camera.type = CAMERA_PERSPECTIVE;
	SetCameraMode(camera, CAMERA_CUSTOM);

	shader = LoadShader("src/libs/lighting.vs", "src/libs/lighting.fs");
	shader.locs[LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
	shader.locs[LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
	shader.locs[LOC_MAP_NORMAL] = GetShaderLocation(shader, "normalMap");
	int ambientLoc = GetShaderLocation(shader, "ambient");
	float f[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	SetShaderValue(shader, ambientLoc, f, UNIFORM_VEC4);
	float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
	SetShaderValue(shader, shader.locs[LOC_VECTOR_VIEW], cameraPos, UNIFORM_VEC3);

	lightColors[0] = { 153, 74, 0, 255 };
	lightColors[1] = { 194, 100, 0, 255 };
	lightColors[2] = ORANGE;
	//lights[0] = CreateLight(LIGHT_POINT, { 14.0f, 5.0f, 0.0f }, Vector3Zero(), {237, 73, 2}, shader);
	//lights[0] = CreateLight(LIGHT_DIRECTIONAL, { 7.0f, 10.0f, 0.0f }, { 6.0f, 0.0f, 0.0f }, {255, 190, 190}, shader);
	lights[0] = CreateLight(LIGHT_POINT, { 14.4f, 1.5f, 0.7f }, Vector3Zero(), lightColors[0], shader);
	lights[3] = CreateLight(LIGHT_POINT, { 0.3f, 1.5f, 0.7f }, Vector3Zero(), lightColors[0], shader);
	UpdateLightValues(shader, lights[0]);
	UpdateLightValues(shader, lights[3]);

	lights[1] = CreateLight(LIGHT_POINT, { flashx, 0.0f, flashy }, Vector3Zero(), GOLD, shader);
	lights[1].enabled = false;
	lights[2] = CreateLight(LIGHT_POINT, rocket.position, Vector3Zero(), { 255, 80, 0 }, shader);
	lights[2].enabled = false;

	Texture stone = LoadTextureFromImage(LoadImage("src/Assets/stone.png"));
	Texture par = LoadTextureFromImage(LoadImage("src/Assets/depth.png"));

	Image goldIm = GenImagePerlinNoise(64, 64, 1.0f, 1.0f, 1.0f);
	ImageColorTint(&goldIm, GOLD);
	Texture gold = LoadTextureFromImage(goldIm);


	for (int i = 0; i < 5; i++) {
		rockModels[i] = LoadModelFromMesh(utils.GenRock());
		rockModels[i].materials[0].maps[MAP_DIFFUSE].texture = stone;
		rockModels[i].materials[0].maps[MAP_NORMAL].texture = par;
		rockModels[i].materials[0].shader = shader;
	}

	for (int i = 0; i < 3; i++) {
		wallModels[i] = LoadModelFromMesh(utils.GenWall());
		wallModels[i].materials[0].maps[MAP_DIFFUSE].texture = stone;
		wallModels[i].materials[0].shader = shader;
	}
	/*wallModels[0].transform = MatrixMultiply(MatrixMultiply(MatrixScale(15.0f, 2.5f, 24.0f), MatrixRotate({ 0.0f, 0.0f, 0.0f }, 0.0f)), MatrixTranslate(0.0f, -1.5f, -12.0f));
	wallModels[1].transform = MatrixMultiply(MatrixMultiply(MatrixScale(14.0f, 2.5f, 35.0f), MatrixRotate({ 0.0f, 0.0f, 1.0f }, 70.0f * DEG2RAD)), MatrixTranslate(18.0f, -7.0f, -17.0f));
	wallModels[2].transform = MatrixMultiply(MatrixMultiply(MatrixScale(14.0f, 2.5f, 35.0f), MatrixRotate({ 0.0f, 0.0f, 1.0f }, -70.0f * DEG2RAD)), MatrixTranslate(-7.0f, 7.0f, -17.0f));
	*/
	wallModels[0].transform = MatrixMultiply(MatrixMultiply(MatrixScale(15.0f, 2.5f, 35.0f), MatrixRotate({ 0.0f, 0.0f, 0.0f }, 0.0f)), MatrixTranslate(0.0f, -1.5f, -20.0f));
	wallModels[1].transform = MatrixMultiply(MatrixMultiply(MatrixScale(15.0f, 2.5f, 35.0f), MatrixRotate({ 0.0f, 0.0f, 1.0f }, 70.0f * DEG2RAD)), MatrixTranslate(15.0f, -1.5f, -20.0f));
	wallModels[2].transform = MatrixMultiply(MatrixMultiply(MatrixScale(15.0f, 2.5f, 35.0f), MatrixRotate({ 0.0f, 0.0f, 1.0f }, -70.0f * DEG2RAD)), MatrixTranslate(-5.3f,  12.5f, -20.0f));

	ingotModel = LoadModel("src/Assets/ingot.obj");
	ingotModel.transform = MatrixMultiply(MatrixMultiply(MatrixMultiply(MatrixRotate({ 1.0f, 0.0f, 0.0f }, 180.0f * DEG2RAD), MatrixRotate({ 0.0f, 1.0f, 0.0f }, 180.0f * DEG2RAD)),
		MatrixRotate({ 0.0f, 0.0f, 1.0f }, 90 * DEG2RAD)), MatrixScale(0.03f, 0.03f, 0.05f));
	ingotModel.materials[0].maps[MAP_DIFFUSE].texture = gold;
	ImageColorGrayscale(&goldIm);
	ingotModel.materials[0].maps[MAP_SPECULAR].texture = LoadTextureFromImage(goldIm);
	ingotModel.materials[0].shader = shader;

	smallGem = LoadModelFromMesh(utils.GenGem(SMALL));
	smallGem.materials[0].maps[MAP_DIFFUSE].color = utils.RequestGemColor();
	smallGem.materials[0].shader = shader;

	mediumGem = LoadModelFromMesh(utils.GenGem(MEDIUM));
	mediumGem.materials[0].maps[MAP_DIFFUSE].color = utils.RequestGemColor();
	mediumGem.materials[0].shader = shader;

	largeGem = LoadModelFromMesh(utils.GenGem(LARGE));
	largeGem.materials[0].maps[MAP_DIFFUSE].color = utils.RequestGemColor();
	largeGem.materials[0].shader = shader;

	//shadow = LoadTexture("src/Assets/shadow.png");
	//shadowXs[0] = GetRandomValue(5, 50) / GetRandomValue(1, 4);

	ballModel = LoadModelFromMesh(utils.GenBall());
	ballModel.materials[0].maps[MAP_DIFFUSE].texture = LoadTextureFromImage(LoadImage("src/Assets/iron.png"));
	ballModel.materials[0].shader = shader;
	

	Image im = LoadImage("src/Assets/pad.png");
	Texture padTexture = LoadTextureFromImage(im);
	padModel = LoadModelFromMesh(GenMeshCube(pad.size, 1.0f, 0.5f));
	padModel.materials[0].maps[MAP_DIFFUSE].texture = padTexture;
	padModel.materials[0].shader = shader;

	Image im1 = ImageFromImage(im, { 100, 100, 64, 64 });
	Texture woodTexture = LoadTextureFromImage(im1);
	torchModel = LoadModelFromMesh(GenMeshCube(0.3f, 0.1f, 2.0f));
	torchModel.materials[0].maps[MAP_DIFFUSE].texture = woodTexture;
	torchModel.materials[0].shader = shader;

	rocketModel = LoadModelFromMesh(utils.GenRocket());
	rocketModel.materials[0].maps[MAP_DIFFUSE].texture = LoadTextureFromImage(LoadImage("src/Assets/Rocket.png"));
	rocketModel.materials[0].shader = shader;

	/*dynamiteModel = LoadModel("src/Assets/dynamite.obj");
	dynamiteModel.transform = MatrixScale(0.05f, 0.05f, 0.05f);
	dynamiteModel.materials[0].maps[MAP_DIFFUSE].texture = LoadTextureFromImage(GenImageColor(2, 2, RED));
	dynamiteModel.materials[0].shader = shader;*/

	hit1 = LoadSound("src/Assets/hit1.ogg");
	hit2 = LoadSound("src/Assets/hit2.ogg");
	rolling = LoadSound("src/Assets/rolling.wav");
	ingot = LoadSound("src/Assets/ingot.mp3"); 
	glass1 = LoadSound("src/Assets/glass1.wav");
	glass2 = LoadSound("src/Assets/glass2.wav");
	jewelry = LoadSound("src/Assets/jewelry.mp3");
	avalanche = LoadSound("src/Assets/avalanche.wav");
	shift = LoadSound("src/Assets/shift.wav");
	pickaxe1 = LoadSound("src/Assets/pickaxe1.wav");
	pickaxe2 = LoadSound("src/Assets/pickaxe2.wav");
	wall1 = LoadSound("src/Assets/wall1.mp3");
	wall2 = LoadSound("src/Assets/wall2.wav");
	rocket_start = LoadSound("src/Assets/rocket_start.wav");
	explosion = LoadSound("src/Assets/explosion.wav");
	wood = LoadSound("src/Assets/wood.ogg");

	ambience = LoadMusicStream("src/Assets/ambience.ogg");
	ambience.looping = true;

	Restart();
}

void GameScreen::Render() {
	
	

	if (flash) {
		lights[1].enabled = true;
		lights[1].position = { flashx, 0.0f, flashy };
		lights[1].target = { flashx, -1.0f, flashy };
		UpdateLightValues(shader, lights[1]);
	}
	else if (lights[1].enabled) {
		lights[1].enabled = false;
		UpdateLightValues(shader, lights[1]);
	}

	if (rocket.explode) {
		lights[2].enabled = true;
		lights[2].position = { rocket.position.x, 0.0f, rocket.position.z };
		lights[2].target = { rocket.position.x, -1.0f, rocket.position.z };
		UpdateLightValues(shader, lights[2]);
	}
	else if (lights[2].enabled) {
		lights[2].enabled = false;
		UpdateLightValues(shader, lights[2]);
	}
	
	

	BeginMode3D(camera);
	for (int i = 0; i < 3; i++) {
		DrawModel(wallModels[i], Vector3Zero(), 1.0f, DARKBROWN);
	}
	RenderParticles();
	utils.DrawScore(score, 1.5f, -2.0f);
	for (int i = 0; i < bricks.size(); i++) {
		if (bricks[i].toughness != 0) {
			switch (bricks[i].type) {
			case 0: DrawModelEx(rockModels[bricks[i].variant], { bricks[i].position.x, 0.0f, bricks[i].position.y },
					{ 0.0f, 0.0f, 1.0f }, bricks[i].angle, Vector3One(), DARKGRAY);
					break;
			case 1: DrawModelEx(ingotModel, { bricks[i].position.x, 0.0f, bricks[i].position.y },
					{ 0.0f, 0.0f, 1.0f }, bricks[i].angle, Vector3One(), WHITE);
					break;
			case 2: DrawModelEx(smallGem, { bricks[i].position.x, 0.0f, bricks[i].position.y },
					{ 0.0f, 0.0f, 1.0f }, bricks[i].angle, Vector3One(), WHITE);
					break;
			case 3: DrawModelEx(mediumGem, { bricks[i].position.x, 0.0f, bricks[i].position.y },
				{ 0.0f, 0.0f, 1.0f }, bricks[i].angle, Vector3One(), WHITE);
				break;
			case 4: DrawModelEx(largeGem, { bricks[i].position.x, 0.0f, bricks[i].position.y },
				{ 0.0f, 0.0f, 1.0f }, bricks[i].angle, Vector3One(), WHITE);
				break;
			}
		}
	}
	if (rocket.isAlive) DrawModel(rocketModel, rocket.position, 1.0f, WHITE);
	DrawModel(padModel, { pad.position.x, 0.0f, pad.position.y }, 1.0f, WHITE);
	DrawModel(ballModel, { ball.position.x, 0.0f, ball.position.y }, 1.0f, WHITE);
	DrawModelEx(torchModel, { 15.0f, 1.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, -45.0f, Vector3One(), WHITE);
	DrawModelEx(torchModel, { -0.5f, 1.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, Vector3One(), WHITE);
	EndMode3D();

	/*DrawText(FormatText("%i", explosiveCooldown), 10, 50, 50, RED);
	DrawText(FormatText("%f", ball.position.x), 10, 100, 50, RED);
	DrawText(FormatText("%f", ball.position.y), 10, 150, 50, RED);*/
}

bool GameScreen::Update(bool pause) {
	if (!pause && start) {
		UpdateMusicStream(ambience);
		timer += GetFrameTime();

		MoveModels();
		UpdateParticles();

		RequestSparks(14.4f, 1.5f, 0.7f, 0.0f, -0.3f * GetRandomValue(1, 5) / GetRandomValue(-5, 5), TORCH);
		RequestSparks(0.1f, 1.5f, 0.7f, 0.0f, -0.3f * GetRandomValue(1, 5) / GetRandomValue(-5, 5), TORCH);
		//	RequestSparks(pad.position.x, 0.5f, pad.position.y + 1.0f, 0.5f * GetRandomValue(-2, 2) / GetRandomValue(-5, 5), -0.3f * GetRandomValue(1, 5) / GetRandomValue(-5, 5), TORCH);

		for (int i = 0; i < bricks.size(); i++) {
			if (bricks[i].position.y <= pad.position.y + 1.0f && bricks[i].toughness != 0) {
				gameOver = true;
				break;
			}
			else gameOver = false;
		}

		if (shakeTimer > 0) {
			utils.ShakeCamera(camera);
			if(GetRandomValue(1, 100) % 11 == 0) RequestShatters(GetRandomValue(1, 10), HEIGHT, ball.dx * 1.5f, ball.dy * 1.5f);
			shakeTimer--;
		}

		if (shakeTimer == 0 && camera.position.x != 7.0f && camera.position.z != 0.0f) {
			camera.position = { 7.0f, 10.0f, 0.0f };
			camera.target = { 7.0f, 0.0f, 0.0f };
			rockSpeed = 0.2f;
		} 

		if (timer >= 5.0f) {
			timer = 0.0f;
			if (stopGenTimer > 0) stopGenTimer--; else if (stopGen) stopGen = false;
			GenNewBricks();
			if (!stopGen) {
				char en = GetRandomValue(0, 100);
				if (en % 2 == 0) RandomEncounter(NOTHING);
				else if (en % 7 == 0) RandomEncounter(AVALANCHE);
				else if (en % 15 == 0) RandomEncounter(SHIFT);
				else RandomEncounter(NOTHING);
			}
		}

		
		if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
			if (GetMouseX() < GetWorldToScreen({ pad.position.x, 0.0f, pad.position.y }, camera).x &&
				GetMouseY() > GetWorldToScreen({ pad.position.x, 0.0f, pad.position.y }, camera).y && pad.position.x + pad.size / 2.0f < 14.0f) {
				pad.getX() += 20.0f * GetFrameTime();
			}
			if (GetMouseX() > GetWorldToScreen({ pad.position.x, 0.0f, pad.position.y }, camera).x &&
				GetMouseY() > GetWorldToScreen({ pad.position.x, 0.0f, pad.position.y }, camera).y && pad.position.x - pad.size / 2.0f > 0.0f) {
				pad.getX() -= 20.0f * GetFrameTime();
			}
		}
		if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
			if (!rocket.isAlive && explosiveCooldown == 0) {
				if (GetMouseY() < GetWorldToScreen({ pad.position.x, 0.0f, pad.position.y }, camera).y) {
					explosiveCooldown = 10;
					lights[0].color = lightColors[0];
					lights[3].color = lightColors[0];
					UpdateLightValues(shader, lights[0]);
					UpdateLightValues(shader, lights[3]);
					rocket.position.x = pad.position.x;
					rocket.direction = (GetWorldToScreen({ pad.position.x, 0.0f, pad.position.y }, camera).x - GetMouseX()) / 50.0f;
					rocket.position.y = 3.0f;
					rocket.position.z = pad.position.y - 6.0f;
					rocket.isAlive = true;
					rocket.lightTime = 0.3f;
					PlaySound(rocket_start);
				}
			}
		}
		

		if (ball.position.x < 0) ball.getX() += GetFrameTime();



		for (int i = 0; i < bricks.size(); i++) {
			if (bricks[i].position.y <= 10.0f) {

				if (rocket.isAlive) {
					if (CheckCollisionRecs({ rocket.position.x + 0.5f, rocket.position.z - 0.5f, 1.0f, 1.0f },
						{ bricks[i].position.x + 0.5f, bricks[i].position.y - 0.5f, 1.0f, 1.0f })
						&& bricks[i].toughness != 0) {
						RequestSparks(rocket.position.x, rocket.position.y, rocket.position.z, 0, 0, EXPLOSION);
						rocket.speed = 1.0f;
						rocket.isAlive = false;
						rocket.explode = true;
						PlaySound(explosion);
						Explosion({ rocket.position.x, rocket.position.z }, rocket.r);
					}
				}

				if (CheckCollisionRecs({ ball.position.x + ball.r, ball.position.y - ball.r, ball.r*2.0f, ball.r*2.0f },
					{ bricks[i].position.x + 0.5f, bricks[i].position.y - 0.5f, 1.0f, 1.0f })
					&& bricks[i].toughness != 0) {

					if (explosiveCooldown <= 1 && lights[0].color.r != lightColors[2].r) {
						lights[0].color = lightColors[2];
						lights[3].color = lightColors[2];
						UpdateLightValues(shader, lights[0]);
						UpdateLightValues(shader, lights[3]);
					}
					else if (explosiveCooldown <= 6 && explosiveCooldown != 0 && lights[0].color.r != lightColors[1].r) {
						lights[0].color = lightColors[1];
						lights[3].color = lightColors[1];
						UpdateLightValues(shader, lights[0]);
						UpdateLightValues(shader, lights[3]);
					}

					if (ball.position.y >= bricks[i].position.y - 0.5f || ball.position.y <= bricks[i].position.y + 0.5f)
						ball.BrickCollision(COLL_U);
					if (bricks[i].toughness == 1) {
						if (ball.position.x >= bricks[i].position.x || ball.position.x <= bricks[i].position.x) ball.BrickCollision(COLL_L);
					}
					switch (bricks[i].type) {
					case 0: RequestShatters(bricks[i].position.x, bricks[i].position.y,
						ball.dx, ball.dy);
						GetRandomValue(1, 6) % 2 ? PlaySound(hit1) : PlaySound(hit2);
						break;
					case 1: RequestSparks(bricks[i].position.x, 0.0f, bricks[i].position.y,
						ball.dx, ball.dy, SPLASH);
						PlaySound(ingot);
						break;
					case 2: RequestShards(bricks[i].position.x, bricks[i].position.y,
						ball.dx, ball.dy);
						GetRandomValue(1, 6) % 2 ? PlaySound(glass1) : PlaySound(glass2);
						PlaySound(jewelry);
						break;
					case 3: RequestShards(bricks[i].position.x, bricks[i].position.y,
						ball.dx, ball.dy);
						GetRandomValue(1, 6) % 2 ? PlaySound(glass1) : PlaySound(glass2);
						PlaySound(jewelry);
						break;
					case 4: RequestShards(bricks[i].position.x, bricks[i].position.y,
						ball.dx, ball.dy);
						GetRandomValue(1, 6) % 2 ? PlaySound(glass1) : PlaySound(glass2);
						PlaySound(jewelry);
						break;
					default: RequestSparks(bricks[i].position.x, 0.0f,  bricks[i].position.y,
						ball.dx, ball.dy, SPLASH);
						break;
					}
					bricks[i].toughness--;
					score += 10;
					if(explosiveCooldown > 0) explosiveCooldown--;
					break;
				}
			}
		}

		if (ball.Update(pad.position, pad.size)) {
			RequestSparks(ball.position.x, 1.0f,  ball.position.y,
				ball.dx / 5.0f, ball.dy / 2.0f, BURST);
			GetRandomValue(1, 6) % 2 ? PlaySound(wall1) : PlaySound(wall2);
			flash = true;
			flashx = ball.position.x;
			flashy = ball.position.y;
		}

		if (ball.hitPad) PlaySound(wood);
		
		if (rocket.explode) {
			rocket.lightTime -= 1.1f * GetFrameTime();
			if (rocket.lightTime < 0.0f) rocket.explode = false;
		}

		if (gameOver || ball.position.y < pad.position.y - 2.0f) return false; else return true;
	}
	if (!pause && !start) {
		ball.getX() = pad.position.x;
		ball.getZ() = pad.position.y + 1.0f;
		if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
			if (GetMouseX() < GetWorldToScreen({ pad.position.x, 0.0f, pad.position.y }, camera).x &&
				pad.position.x + pad.size / 2.0f < 14.0f) {
				pad.getX() += 25.0f * GetFrameTime();
			}
			if (GetMouseX() > GetWorldToScreen({ pad.position.x, 0.0f, pad.position.y }, camera).x &&
				pad.position.x - pad.size / 2.0f > 0.0f) {
				pad.getX() -= 25.0f * GetFrameTime();
			}
		}
		if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
			ball.dx = (float)GetRandomValue(-6, 6);
			ball.dy = (float)GetRandomValue(5, 7);
			start = true;
			PlayMusicStream(ambience);
		}
		return true;
	}
}

void GameScreen::MoveModels() {
	if (rocket.isAlive) {
		RequestSparks(rocket.position.x, rocket.position.y, rocket.position.z, 0.5f, -1.0f, BURST);
		rocket.position.z += rocket.speed * GetFrameTime() / 2.0f;
		if (rocket.position.y > 0.0f) rocket.position.y -= rocket.speed * GetFrameTime() / 10.0f;
		if(rocket.position.x < 14.0f && rocket.position.x > 0.0f) rocket.position.x += rocket.direction * GetFrameTime();
		rocket.speed += 0.5f;
		if (rocket.position.z > 11.0f) {
			rocket.isAlive = false;
			rocket.speed = 1.0f;
		}
	}

	for (int i = 0; i < bricks.size(); i++) {
		if (bricks[i].toughness != 0) {
			bricks[i].position.y -= rockSpeed * GetFrameTime();
			bricks[i].Rotate();
		};
	}
}

void GameScreen::GenNewBricks() {
	if (!stopGen) {
		char n = 0;
		char j = 15;
		for (int i = 0; i < bricks.size(); i++) {
			if (bricks[i].toughness == 0 && n < 7 && j >= 7) {
				n++;
				j--;
				if (GetRandomValue(1, 6) % 2 == 0) {
					bricks[i].position = { (float)n, HEIGHT + 1.0f };
					bricks[i].toughness = 1;
					bricks[i].type = 0;
					if (GetRandomValue(1, 6) % 6 == 0) {
						bricks[i].toughness = 2;
						bricks[i].type = 1;
					}
					if (GetRandomValue(1, 100) % 22 == 0) {
						bricks[i].toughness = 3;
						bricks[i].type = 2;
					}
					if (GetRandomValue(1, 100) % 33 == 0) {
						bricks[i].toughness = 4;
						bricks[i].type = 3;
					}
					if (GetRandomValue(1, 100) % 44 == 0) {
						bricks[i].toughness = 5;
						bricks[i].type = 4;
					}
				}
				if (GetRandomValue(1, 10) % 3 == 0) {
					bricks[i].position = { (float)j, HEIGHT + 1.0f };
					bricks[i].toughness = 1;
					bricks[i].type = 0;
					if (GetRandomValue(1, 6) % 6 == 0) {
						bricks[i].toughness = 2;
						bricks[i].type = 1;
					}
					if (GetRandomValue(1, 100) % 22 == 0) {
						bricks[i].toughness = 3;
						bricks[i].type = 2;
					}
					if (GetRandomValue(1, 100) % 33 == 0) {
						bricks[i].toughness = 4;
						bricks[i].type = 3;
					}
					if (GetRandomValue(1, 100) % 44 == 0) {
						bricks[i].toughness = 5;
						bricks[i].type = 4;
					}
				}
			}
		}
	}
}

void GameScreen::RandomEncounter(int type) {
	if (type == NOTHING) {
		GetRandomValue(1, 6) % 2 ? PlaySound(pickaxe1) : PlaySound(pickaxe2);
		RequestSparks(GetRandomValue(0, 26) / 2.0f, 0.0f, -10.0f, GetRandomValue(1, 100) / 2.0f, 5.0f, SPLASH);
	}
	if (type == AVALANCHE) {
		PlaySound(avalanche);
		stopGen = true;
		stopGenTimer = 10;
		RequestShatters(GetRandomValue(4, 8), HEIGHT, ball.dx, ball.dy);
		RequestShatters(GetRandomValue(4, 8), HEIGHT, ball.dx, ball.dy);
		float n = (float) GetRandomValue(3, 5);
		int column = 0;
		for (int i = 0; i < bricks.size(); i++) {
			if (bricks[i].toughness == 0) {
				bricks[i].position = {(float)column, HEIGHT + 1.0f + n };
				bricks[i].toughness = 1;
				bricks[i].type = 0;
				column++;
				if (column == WIDTH) {
					column = 0;
					n--;
					if (n == 0) break; 
				}
			}
		}
	}
	if (type == SHIFT) {
		PlaySound(shift);
		shakeTimer = 120;
		rockSpeed = 1.0f;
	}
}

void GameScreen::RenderParticles() {
	for (int i = 0; i < particles.size(); i++) {
		if (particles[i].isAlive) {
			switch (particles[i].type) {
			case SHATTER: {
				DrawModel(rockModels[particles[i].variant], { particles[i].position.x,  particles[i].position.y,  particles[i].position.z }, 0.5f, WHITE);
				break;
				}
			case SPARK: {
				BeginBlendMode(BLEND_ADDITIVE);
				if(particles[i].lifeTime < 0.05f)
					DrawPlane(particles[i].position, { 0.1f, 0.1f }, {255, 0, 0, 255});
				else if(particles[i].lifeTime < 0.15f)
					DrawPlane(particles[i].position, { 0.1f, 0.1f }, ORANGE);
				else DrawPlane(particles[i].position, { 0.1f, 0.1f }, GOLD);
				EndBlendMode();
				break;
			}
			case SHARD: {
				BeginBlendMode(BLEND_ADDITIVE);
				DrawTriangle3D(particles[i].position,
				{ particles[i].position.x + GetRandomValue(-2, 2) * 0.1f, particles[i].position.y + GetRandomValue(-2, 2) * 0.1f, particles[i].position.z + GetRandomValue(-2, 2) * 0.1f },
				{ particles[i].position.x + GetRandomValue(-2, 3) * 0.1f, particles[i].position.y + GetRandomValue(-1, 2) * 0.1f, particles[i].position.z + GetRandomValue(-4, 2) * 0.1f },
				utils.RequestGemColor());
				EndBlendMode();
				break;
			}
			/*case FLARE: {
				utils.DrawPlaneTexture(flareTexture, particles[i].position, { 0.1f, 0.1f }, WHITE);
			}*/
			}
		}
	}
}

void GameScreen::UpdateParticles() {

	for (int i = 0; i < particles.size(); i++) {
		if (particles[i].isAlive) {
			switch (particles[i].type) {
			case SHATTER: {
				particles[i].position.x += particles[i].direction.x * GetFrameTime();
				particles[i].position.y += particles[i].direction.y * GetFrameTime();
				particles[i].position.z += particles[i].direction.z * GetFrameTime();

				if (particles[i].position.x >= 15.0f || particles[i].position.x <= 0.0f) {
					particles[i].direction.x *= -1.0f;
				}
				if (particles[i].position.y >= -1.5f) particles[i].position.y *= -1;

				if (particles[i].direction.x >= 0.0f) particles[i].direction.x -= 0.1f;
				if (particles[i].direction.x < 0.0f) particles[i].direction.x += 0.1f;

				if (particles[i].position.z <= -10.0f) {
					particles[i].isAlive = false;
					particles[i].direction = { 0 };
					particles[i].position = { 0 };
					particles[i].type = -1;
				}
				break;
			}
			case SPARK: {
				particles[i].position.x += particles[i].direction.x * GetFrameTime();
				particles[i].position.y += particles[i].direction.y * GetFrameTime();
				particles[i].position.z += (particles[i].direction.z - 0.1f * GetFrameTime()) * GetFrameTime();

				if (particles[i].position.x >= 15.0f || particles[i].position.x <= -1.0f) {
					particles[i].direction.x *= -1.0f;
				}
				//if (particles[i].position.y >= -1.5f) particles[i].position.y *= -1;
				if (particles[i].direction.x >= 0.0f) particles[i].direction.x -= 0.1f;
				if (particles[i].direction.x < 0.0f) particles[i].direction.x += 0.1f;

				particles[i].lifeTime -= GetFrameTime();
				if (particles[i].lifeTime <= 0.0f) {
					particles[i].isAlive = false;
					flash = false;
					//if(rocket.isAlive)rocket.explode = false;
					particles[i].direction = { 0 };
					particles[i].position = { 0 };
					particles[i].type = -1;
				}
				break;
			}
			case SHARD: {
				particles[i].position.x += particles[i].direction.x * GetFrameTime() * 0.3f;
				particles[i].position.y += particles[i].direction.y * GetFrameTime() * 0.5f;
				particles[i].position.z += particles[i].direction.z * GetFrameTime() * 0.1f;

				if (particles[i].position.x >= 15.0f || particles[i].position.x <= 0.0f) {
					particles[i].direction.x *= -1.0f;
				}
				if (particles[i].position.y >= -1.5f) particles[i].position.y *= -1;

				particles[i].lifeTime -= GetFrameTime();
				if (particles[i].lifeTime <= 0.0f) {
					particles[i].isAlive = false;
					flash = false;
					particles[i].direction = { 0 };
					particles[i].position = { 0 };
					particles[i].type = -1;
				}
				break;
			}
			/*case FLARE: {
				particles[i].position.x += particles[i].direction.x * GetFrameTime();
				particles[i].position.y += particles[i].direction.y * GetFrameTime();
				particles[i].position.z += particles[i].direction.z * GetFrameTime();
				if (particles[i].position.x >= 15.0f || particles[i].position.x <= 0.0f) {
					particles[i].direction.x *= -1.0f;
				}
				particles[i].lifeTime -= GetFrameTime();
				if (particles[i].lifeTime <= 0.0f) {
					particles[i].isAlive = false;
					particles[i].direction = { 0 };
					particles[i].position = { 0 };
					particles[i].type = -1;
				}
				break;
			}*/
			}
		}
	}
}

void GameScreen::RequestShatters(float x, float y, float dx, float dy) {
	int n = GetRandomValue(2, 4);
	int ready = 0;
	//flashx = x; flashy = y;
	for (int i = 0; i < particles.size(); i++) {
		if (!particles[i].isAlive) {
			ready++;
			particles[i].type = SHATTER;
			particles[i].isAlive = true;
			particles[i].position = { x, 0.0f, y };
			particles[i].direction = { dx * 3.0f * GetRandomValue(-2, 2) / 1.5f , GetRandomValue(1, 2) * 2.0f, abs(dy) * GetRandomValue(-3, -2) };
			particles[i].variant = GetRandomValue(0, 4);
			if (ready == n) {
				break;
			}
		}
	}
}

void GameScreen::RequestSparks(float x, float y, float z, float dx, float dy, char form) {
	int n = GetRandomValue(40, 60);
	if (form == EXPLOSION) n *= 2;
	if (form == TORCH) n /= (explosiveCooldown*2 + 1);
	int ready = 0;
	for (int i = 0; i < particles.size(); i++) {
		if (!particles[i].isAlive) {
			ready++;
			particles[i].type = SPARK;
			particles[i].isAlive = true;
			particles[i].position = { x, y, z };
			//if (rocket.isAlive)particles[i].position.y = rocket.position.y;
			switch (form) {
			case BURST: particles[i].direction = { dx * GetRandomValue(-3, 3), 0.1f, -dy * GetRandomValue(4, 6) };
						particles[i].lifeTime = 0.2f;
						break;
			case SPLASH: particles[i].direction = { dx * GetRandomValue(-6, 6) / GetRandomValue(-15, 15), GetRandomValue(-4, 4) * 1.5f / GetRandomValue(-6, 6), dy / GetRandomValue(1, 6) };
						 particles[i].lifeTime = 0.3f;
						 break;
			case EXPLOSION: particles[i].direction = { (float)GetRandomValue(-10, 10), (float)GetRandomValue(-4, 4), (float)GetRandomValue(-10, 10) };
							particles[i].lifeTime = 0.4f;
							break;
			case TORCH: particles[i].direction = { (float)GetRandomValue(-5, 5) / GetRandomValue(1, 30), (float)GetRandomValue(-1, 1), (float)GetRandomValue(1, 10) / GetRandomValue(1, 15) };
						particles[i].lifeTime = 0.2f;
						break;
			}
			if (ready == n) {
				break;
			}
		}
	}
}

void GameScreen::RequestShards(float x, float y, float dx, float dy) {
	int n = GetRandomValue(40, 60);
	int ready = 0;
	for (int i = 0; i < particles.size(); i++) {
		if (!particles[i].isAlive) {
			ready++;
			particles[i].type = SHARD;
			particles[i].isAlive = true;
			particles[i].position = { x, 0.0f, y };
			particles[i].direction = { dx * GetRandomValue(-4, 4) / GetRandomValue(-4, 4), GetRandomValue(-1, 4) * 1.1f, dy * GetRandomValue(4, 6) };
			particles[i].lifeTime = 1.0f;
			if (ready == n) {
				break;
			}
		}
	}
}

/*void GameScreen::RequestFlares(float x, float y, float z, float dx, float dy) {
	int n = GetRandomValue(2, 4);
	int ready = 0;
	for (int i = 0; i < particles.size(); i++) {
		if (!particles[i].isAlive) {
			ready++;
			particles[i].type = FLARE;
			particles[i].isAlive = true;
			particles[i].position = { x, y, z };
			particles[i].direction = { dx * GetRandomValue(-2, 2) / GetRandomValue(-4, 4), GetRandomValue(-1, 4) * 1.1f, dy * GetRandomValue(1, 2) };
			particles[i].lifeTime = 0.5f;
			if (ready == n) {
				break;
			}
		}
	}
}*/

void GameScreen::Explosion(Vector2 center, float r) {
	for (int i = 0; i < bricks.size(); i++) {
		if (bricks[i].toughness != 0) {
			if (Vector2Distance(center, bricks[i].position) <= r) {
				bricks[i].toughness = 0;
				switch (bricks[i].type) {
				case 0: RequestShatters(bricks[i].position.x, bricks[i].position.y,
					ball.dx, ball.dy);
					GetRandomValue(1, 6) % 2 ? PlaySound(hit1) : PlaySound(hit2);
					break;
				case 1: RequestSparks(bricks[i].position.x, 0.0f, bricks[i].position.y,
					ball.dx, ball.dy, SPLASH);
					PlaySound(ingot);
					break;
				case 2: RequestShards(bricks[i].position.x, bricks[i].position.y,
					ball.dx, ball.dy);
					GetRandomValue(1, 6) % 2 ? PlaySound(glass1) : PlaySound(glass2);
					PlaySound(jewelry);
					break;
				case 3: RequestShards(bricks[i].position.x, bricks[i].position.y,
					ball.dx, ball.dy);
					GetRandomValue(1, 6) % 2 ? PlaySound(glass1) : PlaySound(glass2);
					PlaySound(jewelry);
					break;
				case 4: RequestShards(bricks[i].position.x, bricks[i].position.y,
					ball.dx, ball.dy);
					GetRandomValue(1, 6) % 2 ? PlaySound(glass1) : PlaySound(glass2);
					PlaySound(jewelry);
					break;
				default: RequestSparks(bricks[i].position.x, 0.0f, bricks[i].position.y,
					ball.dx, ball.dy, SPLASH);
					PlaySound(jewelry);
					break;
				}
			}
		}
	}
}