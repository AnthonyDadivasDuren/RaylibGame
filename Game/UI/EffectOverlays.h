#pragma once


class EffectOverlays
{
public:
    EffectOverlays() = default;

    static void DrawWave10Flash(float timer, int screenWidth, int screenHeight);
    static void DrawShotgunFlash(float timer, int screenWidth, int screenHeight);
    static void DrawDamageFlash(float hitFlashTimer, int screenWidth, int screenHeight);
};
