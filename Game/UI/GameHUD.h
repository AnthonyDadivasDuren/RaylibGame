#pragma once

class GameManager;
class WaveSystem;
class WeaponSystem;

class GameHUD
{
public:
    GameHUD() = default;

    void Draw(GameManager& gm, const WaveSystem& waveSystem, const WeaponSystem& weaponSystem, int screenWidth, int screenHeight) const;
};
