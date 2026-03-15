#pragma once

#include "GameState.h"
#include "EnemyPool.h"
#include "BulletPool.h"
#include "EnemyFactory.h"
#include "ParticlePool.h"
#include "XpOrbPool.h"
#include "AmmoOrbPool.h"
#include "HealthOrbPool.h"
#include "Game/Upgrades/UpgradeSystem.h"
#include "Game/Upgrades/UpgradeUI.h"
#include "Game/Waves/WaveSystem.h"
#include "Game/Player/Player.h"
#include "Game/Player/DashSystem.h"
#include "Game/Enemies/EnemySpawner.h"
#include "Game/Enemies/BossController.h"
#include "Game/Enemies/EnemySystem.h"
#include "Game/Enemies/ShockwaveSystem.h"
#include "Game/Weapons/WeaponSystem.h"
#include "Game/Companions/CompanionSystem.h"
#include "Game/Orbs/OrbSystem.h"
#include "Game/UI/GameHUD.h"
#include <cmath>
#include <vector>
#include <set>
#include <string>
#include <algorithm>

class PlayingState : public GameState
{
    Player player;
    WaveSystem waveSystem;
    EnemySpawner enemySpawner;
    static const int screenWidth = 1800;
    static const int screenHeight = 1350;
    WeaponSystem weaponSystem;
    bool choosingUpgrade;
    float shotgunFlashTimer;
    bool choosingStartingUpgrade;
    int upgradePicksRemaining;
    int upgradePicksTotal;
    UpgradeSystem upgradeSystem;
    UpgradeUI upgradeUI;
    GameHUD gameHUD;
    DashSystem dashSystem;
    ShockwaveSystem shockwaveSystem;

    BossController bossController;
    EnemySystem enemySystem;
    CompanionSystem companionSystem;
    OrbSystem orbSystem;

public:
    PlayingState();
    void Enter() override;
    void Exit() override;
    std::string Update(float deltaTime, EnemyPool* enemyPool, BulletPool* bulletPool, ParticlePool* particlePool, XpOrbPool* xpOrbPool, AmmoOrbPool* ammoOrbPool, HealthOrbPool* healthOrbPool) override;
    void Draw(EnemyPool* enemyPool, BulletPool* bulletPool, ParticlePool* particlePool, XpOrbPool* xpOrbPool, AmmoOrbPool* ammoOrbPool, HealthOrbPool* healthOrbPool) override;
    const char* GetName() const override;
};