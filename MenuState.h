#pragma once

#include "GameState.h"

class MenuState : public GameState
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
            return "Playing";
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
        const char* title = "KILL CUBE";
        const char* startPrompt = "Press ENTER to Start";
        const int titleFontSize = 90;
        const int hintFontSize = 20;
        int titleX = screenWidth / 2 - MeasureText(title, titleFontSize) / 2;
        int startX = screenWidth / 2 - MeasureText(startPrompt, 25) / 2;
        int centerY = screenHeight / 2;
        DrawText(title, titleX, centerY - 180, titleFontSize, MAROON);
        DrawText(startPrompt, startX, centerY - 20, 25, DARKGRAY);
        int cx = screenWidth / 2;
        DrawText("WASD to move", cx - MeasureText("WASD to move", hintFontSize) / 2, centerY + 35, hintFontSize, GRAY);
        DrawText("Space to dash (dash to gain shotgun ammo)", cx - MeasureText("Space to dash (dash to gain shotgun ammo)", hintFontSize) / 2, centerY + 58, hintFontSize, GRAY);
        DrawText("Q to switch weapons", cx - MeasureText("Q to switch weapons", hintFontSize) / 2, centerY + 81, hintFontSize, GRAY);
        const char* pistolHint = "Pistol auto-shoots in movement direction";
        DrawText(pistolHint, cx - MeasureText(pistolHint, hintFontSize) / 2, centerY + 104, hintFontSize, GRAY);
        DrawText("Left click to shoot shotgun (spawns health orbs)", cx - MeasureText("Left click to shoot shotgun (spawns health orbs)", hintFontSize) / 2, centerY + 127, hintFontSize, GRAY);
    }

    const char* GetName() const override { return "Menu"; }
};