#include "UpgradeUI.h"
#include "UpgradeSystem.h"
#include "raylib.h"

int UpgradeUI::HandleUpgradeInput(UpgradeSystem& upgradeSystem)
{
    if (IsKeyPressed(KEY_ONE) && upgradeSystem.GetOfferCount() >= 1 && upgradeSystem.GetOffer(0) >= 0)
    {
        upgradeSystem.ApplyUpgrade(upgradeSystem.GetOffer(0));
        return 1;
    }
    if (IsKeyPressed(KEY_TWO) && upgradeSystem.GetOfferCount() >= 2 && upgradeSystem.GetOffer(1) >= 0)
    {
        upgradeSystem.ApplyUpgrade(upgradeSystem.GetOffer(1));
        return 2;
    }
    if (IsKeyPressed(KEY_THREE) && upgradeSystem.GetOfferCount() >= 3 && upgradeSystem.GetOffer(2) >= 0)
    {
        upgradeSystem.ApplyUpgrade(upgradeSystem.GetOffer(2));
        return 3;
    }
    return 0;
}

void UpgradeUI::DrawStartingUpgradeScreen(UpgradeSystem& upgradeSystem, int screenWidth, int screenHeight) const
{
    int cx = screenWidth / 2;
    int cy = screenHeight / 2;
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));
    DrawText("Choose your starting upgrade:", cx - 200, cy - 100, 28, GOLD);
    if (upgradeSystem.GetOfferCount() >= 1) DrawText(TextFormat("1 - %s", upgradeSystem.GetUpgradeName(upgradeSystem.GetOffer(0))), cx - 250, cy - 40, 22, WHITE);
    if (upgradeSystem.GetOfferCount() >= 2) DrawText(TextFormat("2 - %s", upgradeSystem.GetUpgradeName(upgradeSystem.GetOffer(1))), cx - 250, cy, 22, WHITE);
    if (upgradeSystem.GetOfferCount() >= 3) DrawText(TextFormat("3 - %s", upgradeSystem.GetUpgradeName(upgradeSystem.GetOffer(2))), cx - 250, cy + 40, 22, WHITE);
    DrawText("Press 1, 2, or 3 to choose", cx - 150, cy + 100, 20, LIGHTGRAY);
}

void UpgradeUI::DrawLevelUpScreen(UpgradeSystem& upgradeSystem, int upgradePicksTotal, int upgradePicksRemaining, int screenWidth, int screenHeight) const
{
    int cx = screenWidth / 2;
    int cy = screenHeight / 2;
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));
    if (upgradePicksTotal >= 2)
        DrawText(TextFormat("LEVEL UP! Choose upgrade (%d of %d):", upgradePicksTotal - upgradePicksRemaining + 1, upgradePicksTotal), cx - 220, cy - 100, 28, GOLD);
    else
        DrawText("LEVEL UP! Choose an upgrade:", cx - 180, cy - 100, 28, GOLD);
    if (upgradeSystem.GetOfferCount() >= 1) DrawText(TextFormat("1 - %s", upgradeSystem.GetUpgradeName(upgradeSystem.GetOffer(0))), cx - 250, cy - 40, 22, WHITE);
    if (upgradeSystem.GetOfferCount() >= 2) DrawText(TextFormat("2 - %s", upgradeSystem.GetUpgradeName(upgradeSystem.GetOffer(1))), cx - 250, cy, 22, WHITE);
    if (upgradeSystem.GetOfferCount() >= 3) DrawText(TextFormat("3 - %s", upgradeSystem.GetUpgradeName(upgradeSystem.GetOffer(2))), cx - 250, cy + 40, 22, WHITE);
    DrawText("Press 1, 2, or 3 to choose", cx - 150, cy + 100, 20, LIGHTGRAY);
}
