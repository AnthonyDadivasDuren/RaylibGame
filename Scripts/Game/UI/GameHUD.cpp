#include "Game/UI/GameHUD.h"
#include "Core/GameManager.h"
#include "Game/Waves/WaveSystem.h"
#include "Game/Weapons/WeaponSystem.h"
#include "raylib.h"

void GameHUD::Draw(GameManager& gm, const WaveSystem& waveSystem, const WeaponSystem& weaponSystem, int screenWidth, int screenHeight) const
{
    const int topLeftFontSize = 26;
    const int topLeftLineHeight = 32;
    DrawText(TextFormat("XP: %d / %d  Level %d", gm.GetXp(), gm.GetXpToNextLevel(), gm.GetLevel()), 10, 10, topLeftFontSize, DARKGREEN);
    int sec = (int)gm.GetTime();
    DrawText(TextFormat("Time: %d:%02d", sec / 60, sec % 60), 10, 10 + topLeftLineHeight, topLeftFontSize, DARKGRAY);
    DrawText(TextFormat("Wave: %d", gm.GetWave()), 10, 10 + topLeftLineHeight * 2, topLeftFontSize, MAROON);
    DrawText(TextFormat("Next: %.0fs", waveSystem.GetTimeRemaining()), 10, 10 + topLeftLineHeight * 3, topLeftFontSize - 2, GRAY);

    const int bigFontSize = 32;
    const int bigPadding = 28;
    const int bigLineHeight = 42;
    const int labelGap = 12;
    const int bottomRightLines = 4;
    int rightX = screenWidth - bigPadding;
    int bottomY = screenHeight - bigPadding - bigLineHeight * bottomRightLines;

    int maxLabelW = MeasureText("WEAPON", bigFontSize);
    if (MeasureText("SHOTGUN", bigFontSize) > maxLabelW) maxLabelW = MeasureText("SHOTGUN", bigFontSize);
    if (MeasureText("DASH", bigFontSize) > maxLabelW) maxLabelW = MeasureText("DASH", bigFontSize);
    if (MeasureText("HEALTH", bigFontSize) > maxLabelW) maxLabelW = MeasureText("HEALTH", bigFontSize);
    maxLabelW += labelGap;

    const char* weaponName = (weaponSystem.GetWeapon() == 0) ? "PISTOL" : "SHOTGUN";
    int weaponW = MeasureText(weaponName, bigFontSize);
    DrawText("WEAPON", rightX - maxLabelW - weaponW, bottomY, bigFontSize, GRAY);
    DrawText(weaponName, rightX - weaponW, bottomY, bigFontSize, (weaponSystem.GetWeapon() == 0) ? LIGHTGRAY : ORANGE);

    int shotgunW = MeasureText(TextFormat("%d / %d", gm.GetShotgunAmmo(), gm.GetMaxShotgunAmmo()), bigFontSize);
    DrawText("SHOTGUN", rightX - maxLabelW - shotgunW, bottomY + bigLineHeight, bigFontSize, ORANGE);
    DrawText(TextFormat("%d / %d", gm.GetShotgunAmmo(), gm.GetMaxShotgunAmmo()), rightX - shotgunW, bottomY + bigLineHeight, bigFontSize, WHITE);

    int dashW = MeasureText(TextFormat("%d / %d", gm.GetDashCharges(), gm.GetMaxDashCharges()), bigFontSize);
    DrawText("DASH", rightX - maxLabelW - dashW, bottomY + bigLineHeight * 2, bigFontSize, DARKBLUE);
    DrawText(TextFormat("%d / %d", gm.GetDashCharges(), gm.GetMaxDashCharges()), rightX - dashW, bottomY + bigLineHeight * 2, bigFontSize, gm.GetDashCharges() > 0 ? SKYBLUE : GRAY);

    int healthW = MeasureText(TextFormat("%d / %d", gm.GetHealth(), gm.GetMaxHealth()), bigFontSize);
    DrawText("HEALTH", rightX - maxLabelW - healthW, bottomY + bigLineHeight * 3, bigFontSize, RED);
    DrawText(TextFormat("%d / %d", gm.GetHealth(), gm.GetMaxHealth()), rightX - healthW, bottomY + bigLineHeight * 3, bigFontSize, WHITE);
}
