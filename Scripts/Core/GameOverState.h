#pragma once

#include "Core/GameState.h"

class GameOverState : public GameState
{
public:
    void Enter() override {}
    void Exit() override {}

    std::string Update(float deltaTime, EnemyPool* enemyPool, BulletPool* bulletPool, ParticlePool* particlePool, XpOrbPool* xpOrbPool, AmmoOrbPool* ammoOrbPool, HealthOrbPool* healthOrbPool) override
    {
        (void)deltaTime;
        (void)enemyPool;
        (void)bulletPool;
        (void)particlePool;
        (void)xpOrbPool;
        (void)ammoOrbPool;
        (void)healthOrbPool;
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
            return "Menu";
        return "";
    }

    void Draw(EnemyPool* enemyPool, BulletPool* bulletPool, ParticlePool* particlePool, XpOrbPool* xpOrbPool, AmmoOrbPool* ammoOrbPool, HealthOrbPool* healthOrbPool) override
    {
        (void)enemyPool;
        (void)bulletPool;
        (void)particlePool;
        (void)xpOrbPool;
        (void)ammoOrbPool;
        (void)healthOrbPool;
        const int screenWidth = 1800;
        const int screenHeight = 1350;
        const char* gameOver = "GAME OVER";
        const char* menuPrompt = "Press ENTER to return to Menu";
        int centerY = screenHeight / 2;
        int gameOverX = screenWidth / 2 - MeasureText(gameOver, 40) / 2;
        DrawText(gameOver, gameOverX, centerY - 80, 40, RED);
        const char* timeStr = TextFormat("Time: %d sec", (int)GameManager::Instance().GetTime());
        int timeX = screenWidth / 2 - MeasureText(timeStr, 22) / 2;
        DrawText(timeStr, timeX, centerY - 20, 22, GRAY);
        int menuX = screenWidth / 2 - MeasureText(menuPrompt, 22) / 2;
        DrawText(menuPrompt, menuX, centerY + 40, 22, DARKGRAY);
    }

    const char* GetName() const override { return "GameOver"; }
};
