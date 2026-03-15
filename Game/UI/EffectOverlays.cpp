#include "EffectOverlays.h"
#include "raylib.h"

void EffectOverlays::DrawWave10Flash(float timer, int screenWidth, int screenHeight)
{
    if (timer <= 0.0f) return;
    float alpha = timer / 1.0f;
    if (alpha > 1.0f) alpha = 1.0f;
    DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(WHITE, alpha));
}

void EffectOverlays::DrawShotgunFlash(float timer, int screenWidth, int screenHeight)
{
    if (timer <= 0.0f) return;
    float alpha = (timer / 0.1f) * 0.35f;
    if (alpha > 1.0f) alpha = 1.0f;
    DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(ORANGE, alpha));
}

void EffectOverlays::DrawDamageFlash(float hitFlashTimer, int screenWidth, int screenHeight)
{
    if (hitFlashTimer <= 0.0f) return;
    float alpha = (hitFlashTimer / 0.2f) * 0.5f;
    if (alpha > 1.0f) alpha = 1.0f;
    DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(RED, alpha));
}
