#include "raylib.h"
#include "Core/GameManager.h"
#include "Shared/EnemyFactory.h"
#include "Pools/EnemyPool.h"
#include "Pools/BulletPool.h"
#include "Pools/ParticlePool.h"
#include "Pools/XpOrbPool.h"
#include "Pools/AmmoOrbPool.h"
#include "Pools/HealthOrbPool.h"
#include "Core/GameState.h"
#include "Core/MenuState.h"
#include "Core/PlayingState.h"
#include "Core/GameOverState.h"
#include "Core/StateManager.h"
#include "Core/Viewport.h"

int main()
{
	const int gameWidth = Viewport::GameWidth;
	const int gameHeight = Viewport::GameHeight;

	SetConfigFlags(FLAG_WINDOW_UNDECORATED);
	int monitorW = GetMonitorWidth(0);
	int monitorH = GetMonitorHeight(0);
	InitWindow(monitorW, monitorH, "KILL CUBE");
	SetWindowPosition(0, 0);
	HideCursor();
	SetTargetFPS(240);

	RenderTexture2D gameRender = LoadRenderTexture(gameWidth, gameHeight);

	EnemyPool enemyPool(900);
	BulletPool bulletPool(400, gameWidth, gameHeight);
	ParticlePool particlePool(8000);
	XpOrbPool xpOrbPool(15200);
	AmmoOrbPool ammoOrbPool(128);
	HealthOrbPool healthOrbPool(240);

	StateManager stateManager;
	stateManager.SetPools(&enemyPool, &bulletPool, &particlePool, &xpOrbPool, &ammoOrbPool, &healthOrbPool);
	stateManager.RegisterState("Menu", new MenuState());
	stateManager.RegisterState("Playing", new PlayingState());
	stateManager.RegisterState("GameOver", new GameOverState());
	stateManager.ChangeState("Menu");

	while (!WindowShouldClose())
	{
		float deltaTime = GetFrameTime();
		int winW = GetScreenWidth();
		int winH = GetScreenHeight();
		Viewport::SetViewport(winW, winH);

		int mx = GetMouseX();
		int my = GetMouseY();
		int clampX = mx < 0 ? 0 : (mx >= winW ? winW - 1 : mx);
		int clampY = my < 0 ? 0 : (my >= winH ? winH - 1 : my);
		if (mx != clampX || my != clampY)
			SetMousePosition(clampX, clampY);

		stateManager.Update(deltaTime, &enemyPool, &bulletPool, &particlePool, &xpOrbPool, &ammoOrbPool, &healthOrbPool);

		BeginDrawing();
		ClearBackground(BLACK);

		BeginTextureMode(gameRender);
		ClearBackground(BLACK);
		stateManager.Draw(&enemyPool, &bulletPool, &particlePool, &xpOrbPool, &ammoOrbPool, &healthOrbPool);
		EndTextureMode();

		float scale = Viewport::GetScale();
		float offsetX = Viewport::GetOffsetX();
		float offsetY = Viewport::GetOffsetY();
		Rectangle source = { 0, 0, (float)gameWidth, (float)-gameHeight };
		Rectangle dest = { offsetX, offsetY, (float)gameWidth * scale, (float)gameHeight * scale };
		DrawTexturePro(gameRender.texture, source, dest, { 0, 0 }, 0.0f, WHITE);

		EndDrawing();
	}

	UnloadRenderTexture(gameRender);
	CloseWindow();
	return 0;
}
