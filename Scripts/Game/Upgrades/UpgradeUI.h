#pragma once

class UpgradeSystem;


class UpgradeUI
{
public:
    UpgradeUI() = default;

    int HandleUpgradeInput(UpgradeSystem& upgradeSystem);

    void DrawStartingUpgradeScreen(UpgradeSystem& upgradeSystem, int screenWidth, int screenHeight) const;
    void DrawLevelUpScreen(UpgradeSystem& upgradeSystem, int upgradePicksTotal, int upgradePicksRemaining, int screenWidth, int screenHeight) const;
};
