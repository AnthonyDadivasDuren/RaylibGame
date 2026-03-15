#include "PlayingState.h"
#include "BulletPool.h"
#include "Game/Enemies/EnemySystem.h"
#include "Game/Enemies/BossController.h"
#include "Game/Weapons/WeaponSystem.h"
#include "Game/Upgrades/UpgradeUI.h"
#include "Game/UI/EffectOverlays.h"
#include "Viewport.h"
#include "raylib.h"
#include "GameManager.h"

PlayingState::PlayingState()
    : choosingUpgrade(false)
    , shotgunFlashTimer(0.0f)
    , choosingStartingUpgrade(false)
    , upgradePicksRemaining(1)
    , upgradePicksTotal(1)
{
}

void PlayingState::Enter()
{
    GameManager::Instance().Reset();
    player.Reset(900.0f, 675.0f);
    waveSystem.Reset();
    enemySpawner.Reset();
    choosingUpgrade = false;
    dashSystem.Reset();
    weaponSystem.Reset();
    shotgunFlashTimer = 0.0f;
    choosingStartingUpgrade = true;
    upgradePicksRemaining = 1;
    upgradePicksTotal = 1;
    bossController.ResetWave10State();
    shockwaveSystem.Clear();
    companionSystem.Reset(900.0f, 675.0f);
    upgradeSystem.FillRandomOffers(true);
}

void PlayingState::Exit()
{
}

std::string PlayingState::Update(float deltaTime, EnemyPool* enemyPool, BulletPool* bulletPool, ParticlePool* particlePool, XpOrbPool* xpOrbPool, AmmoOrbPool* ammoOrbPool, HealthOrbPool* healthOrbPool)
{
    GameManager& gm = GameManager::Instance();
    float px = player.GetCenterX();
    float py = player.GetCenterY();

    if (choosingStartingUpgrade)
    {
        int pick = upgradeUI.HandleUpgradeInput(upgradeSystem);
        if (pick != 0) { choosingStartingUpgrade = false; upgradePicksRemaining = 0; return ""; }
        return "";
    }

    if (choosingUpgrade)
    {
        int pick = upgradeUI.HandleUpgradeInput(upgradeSystem);
        if (pick != 0)
        {
            upgradePicksRemaining--;
            if (upgradePicksRemaining <= 0) choosingUpgrade = false;
            else upgradeSystem.FillRandomOffers(false);
            return "";
        }
        return "";
    }

    gm.AddTime(deltaTime);
    waveSystem.AddTime(deltaTime);
    if (waveSystem.IsComplete())
    {
        waveSystem.ResetTimer();
        int w = gm.GetWave();
        if (w == 10 && bossController.IsActive()) { }
        else
        {
            gm.NextWave();
            if (gm.GetWave() == 10)
                bossController.OnWave10WaveComplete(enemyPool, xpOrbPool, &gm);
        }
    }

    bossController.UpdateWave10Transition(deltaTime, enemyPool, xpOrbPool, &gm, (float)screenWidth * 0.5f, (float)screenHeight * 0.5f);

    if (gm.IsGameOver())
    {
        if (enemyPool) enemyPool->ResetPool();
        if (bulletPool) bulletPool->ResetPool();
        if (particlePool) particlePool->ResetPool();
        orbSystem.Reset(xpOrbPool, ammoOrbPool, healthOrbPool);
        return "GameOver";
    }

    if (IsKeyPressed(KEY_Q))
        weaponSystem.SetWeapon(weaponSystem.GetWeapon() == 0 ? 1 : 0);

    player.UpdateTimers(deltaTime);

    if (dashSystem.IsDashing())
    {
        dashSystem.AddTrailPoint(player.GetCenterX(), player.GetCenterY());
        float outDx, outDy;
        dashSystem.Tick(deltaTime, gm.GetDashLength(), gm.GetDashDuration(), outDx, outDy);
        player.AddPosition(outDx, outDy);
        px = player.GetCenterX();
        py = player.GetCenterY();
    }
    else
    {
        dashSystem.ClearTrail();
        dashSystem.UpdateRechargeAndInput(deltaTime, &gm, &player, particlePool);
        player.UpdateMovement(deltaTime, gm.GetPlayerSpeedMultiplier(), screenWidth, screenHeight);
        px = player.GetCenterX();
        py = player.GetCenterY();
    }

    if (dashSystem.IsDashing() && enemyPool && ammoOrbPool)
        dashSystem.ApplyDashDamage(px, py, enemyPool, particlePool, xpOrbPool, ammoOrbPool, &gm, &shockwaveSystem);

    companionSystem.Update(deltaTime, px, py, player.GetLastMoveDirX(), player.GetLastMoveDirY(),
                           screenWidth, screenHeight, enemyPool, bulletPool, xpOrbPool, &gm);

    shockwaveSystem.Update(deltaTime, enemyPool, particlePool, xpOrbPool, &gm);

    int wave = gm.GetWave();
    enemySystem.Update(deltaTime, px, py, wave, enemyPool);

    bool spawningEnabled = !(gm.GetWave() == 10 && (bossController.IsActive() || bossController.IsBossSpawnTriggered()));
    enemySpawner.Update(deltaTime, wave, screenWidth, screenHeight, spawningEnabled, enemyPool);

    bossController.Update(deltaTime, px, py, player.GetSize(), player.IsVulnerable(),
        player.GetLastMoveDirX(), player.GetLastMoveDirY(), gm.GetPlayerSpeedMultiplier(),
        screenWidth, screenHeight, &gm, &player, bulletPool, particlePool, enemyPool);

    if (weaponSystem.TryFirePistol(deltaTime, px, py, player.GetLastMoveDirX(), player.GetLastMoveDirY(), enemyPool, bulletPool, &gm,
            bossController.IsActive(), bossController.GetX(), bossController.GetY()))
        { }
    if (weaponSystem.TryFireShotgun(px, py, bulletPool, particlePool, &gm))
        shotgunFlashTimer = 0.1f;

    if (gm.HasAutoAttackerUpgrade() && bulletPool && enemyPool)
        weaponSystem.TryFireAutoAttacker(deltaTime, px, py, enemyPool, bulletPool,
            bossController.IsActive(), bossController.GetX(), bossController.GetY(), &gm);
    if (shotgunFlashTimer > 0.0f) shotgunFlashTimer -= deltaTime;

    if (bulletPool)
    {
        bulletPool->UpdateAll(deltaTime, px, py);
        bulletPool->HandleBossBigExplosions(px, py);
    }

    if (enemyPool && bulletPool) enemyPool->CheckCollisions(*bulletPool, particlePool, xpOrbPool, healthOrbPool);

    if (bossController.IsActive() && bulletPool)
        bossController.CheckBulletHits(*bulletPool, particlePool, &gm);

    enemySystem.HandlePlayerCollisions(&player, enemyPool, bulletPool, &gm, px, py);

    if (xpOrbPool) xpOrbPool->UpdateAll(deltaTime, px, py);
    if (ammoOrbPool) ammoOrbPool->UpdateAll(deltaTime, px, py);
    if (healthOrbPool) healthOrbPool->UpdateAll(deltaTime, px, py);
    if (particlePool) particlePool->UpdateAll(deltaTime);

    int levelUpPicks = upgradeSystem.CheckLevelUp(gm);
    if (levelUpPicks > 0)
    {
        upgradePicksTotal = levelUpPicks;
        upgradePicksRemaining = levelUpPicks;
        choosingUpgrade = true;
        upgradeSystem.FillRandomOffers(false);
    }

    return "";
}

void PlayingState::Draw(EnemyPool* enemyPool, BulletPool* bulletPool, ParticlePool* particlePool, XpOrbPool* xpOrbPool, AmmoOrbPool* ammoOrbPool, HealthOrbPool* healthOrbPool)
{
    ClearBackground(BLACK);

    EffectOverlays::DrawWave10Flash(bossController.GetWave10FlashTimer(), screenWidth, screenHeight);

    float px = player.GetCenterX();
    float py = player.GetCenterY();

    Color playerColor = BLUE;
    if (player.GetInvincibleTimer() > 0.0f)
        playerColor = ColorAlpha(BLUE, 0.5f);
    else if (player.GetHitFlashTimer() > 0.0f)
    {
        const float total = 0.2f;
        float intensity = 0.0f;
        float hitFlash = player.GetHitFlashTimer();
        if (hitFlash > total - 0.05f)
            intensity = (total - hitFlash) / 0.05f;
        else if (hitFlash < 0.05f)
            intensity = hitFlash / 0.05f;
        else
            intensity = 1.0f;
        playerColor.r = (unsigned char)(BLUE.r + (255 - BLUE.r) * intensity);
        playerColor.g = (unsigned char)(BLUE.g + (255 - BLUE.g) * intensity);
        playerColor.b = (unsigned char)(BLUE.b + (255 - BLUE.b) * intensity);
    }
    float pSize = player.GetSize();
    dashSystem.DrawTrail(pSize);
    DrawRectangleV({ player.GetX(), player.GetY() }, { pSize, pSize }, playerColor);
    companionSystem.Draw();

    if (bulletPool) bulletPool->DrawAll();
    orbSystem.Draw(xpOrbPool, ammoOrbPool, healthOrbPool);
    if (enemyPool) enemyPool->DrawAll();

    bossController.Draw(screenWidth, screenHeight);
    shockwaveSystem.Draw();

    if (particlePool) particlePool->DrawAll();

    EffectOverlays::DrawShotgunFlash(shotgunFlashTimer, screenWidth, screenHeight);
    EffectOverlays::DrawDamageFlash(player.GetHitFlashTimer(), screenWidth, screenHeight);

    GameManager& gmHud = GameManager::Instance();
    gameHUD.Draw(gmHud, waveSystem, weaponSystem, screenWidth, screenHeight);

    if (weaponSystem.GetWeapon() == 1)
    {
        float mx = Viewport::GetGameMouseX();
        float my = Viewport::GetGameMouseY();
        const float size = 8.0f;
        const float thickness = 2.0f;
        DrawLineEx({ mx - size, my }, { mx + size, my }, thickness, WHITE);
        DrawLineEx({ mx, my - size }, { mx, my + size }, thickness, WHITE);
    }

    if (choosingStartingUpgrade)
        upgradeUI.DrawStartingUpgradeScreen(upgradeSystem, screenWidth, screenHeight);
    else if (choosingUpgrade)
        upgradeUI.DrawLevelUpScreen(upgradeSystem, upgradePicksTotal, upgradePicksRemaining, screenWidth, screenHeight);
}

const char* PlayingState::GetName() const
{
    return "Playing";
}
