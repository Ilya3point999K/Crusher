#include <raylib.h>
#include "headers/GameScreen.h"

int main() {
	SetConfigFlags(FLAG_VSYNC_HINT);
	InitWindow(800, 760, "Crusher");
	InitAudioDevice();
	SetTargetFPS(60);


	GameScreen s;
	bool t = false;

	while (!WindowShouldClose()) {
		ClearBackground(GRAY);
		BeginDrawing();
		if (!s.Update(t)) s.Restart();
		s.Render();
		EndDrawing();
		if (IsKeyPressed(KEY_P) && !t) t = true; else if (IsKeyPressed(KEY_P) && t) t = false;
	}
	//TraceLog(LOG_INFO, "Score: ", FormatText("%i", s.score));
	CloseWindow();
	return 0;
}
