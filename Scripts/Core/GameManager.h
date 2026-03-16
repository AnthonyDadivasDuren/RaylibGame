#pragma once

#include <cmath>

class GameManager
{
private:
    GameManager() : playerHealth(150), maxHealth(150), gameTime(0.0f), score(0), wave(1),
        level(1), xp(0), xpToNextLevel(60), damageMultiplier(1.0f), fireRateMultiplier(1.0f),
        hasRicochetUpgrade(false), hasPierceUpgrade(false), bulletSizeMultiplier(1.0f), hasDeathExplosionUpgrade(false),
        explosionRadiusMultiplier(1.0f), explosionChancePercent(5.0f), playerSpeedMultiplier(1.0f),
        hasDashUpgrade(true), dashCooldown(1.5f), dashDuration(0.12f), dashLength(150.0f),
        dashCharges(2), maxDashCharges(2),
        hasRicochetPlusPlusUpgrade(false), ricochetExtraBounces(0), piercePlusCount(0), shotgunAmmo(0), maxShotgunAmmo(7), orbMagnetismMultiplier(1.0f),
        shotgunPelletCountBonus(0), shotgunPelletDamageMultiplier(1.0f), hasAutoAttackerUpgrade(false), autoAttackerRange(200.0f), followGunSpeedMultiplier(1.0f),
        companionCount(0), companionDamageMultiplier(1.0f), companionFireRateMultiplier(1.0f),
        orbCompanionCount(0), orbCompanionSpeed(337.5f), orbCompanionMagnetismMultiplier(1.0f),
        shotgunCompanionCount(0) {}
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    int playerHealth;
    int maxHealth;
    float gameTime;
    int score;
    int wave;
    int level;
    int xp;
    int xpToNextLevel;
    float damageMultiplier;
    float fireRateMultiplier;
    bool hasRicochetUpgrade;
    bool hasPierceUpgrade;
    float bulletSizeMultiplier;
    bool hasDeathExplosionUpgrade;
    float explosionRadiusMultiplier;
    float explosionChancePercent;
    float playerSpeedMultiplier;
    bool hasDashUpgrade;
    float dashCooldown;
    float dashDuration;
    float dashLength;
    int dashCharges;
    int maxDashCharges;
    bool hasRicochetPlusPlusUpgrade;
    int ricochetExtraBounces;
    int piercePlusCount;
    int shotgunAmmo;
    int maxShotgunAmmo;
    float orbMagnetismMultiplier;
    int shotgunPelletCountBonus;
    float shotgunPelletDamageMultiplier;
    bool hasAutoAttackerUpgrade;
    float autoAttackerRange;
    float followGunSpeedMultiplier;
    int companionCount;
    float companionDamageMultiplier;
    float companionFireRateMultiplier;
    int orbCompanionCount;
    float orbCompanionSpeed;
    float orbCompanionMagnetismMultiplier;
    int shotgunCompanionCount;

public:
    static GameManager& Instance()
    {
        static GameManager instance;
        return instance;
    }

    void Reset()
    {
        playerHealth = 150;
        maxHealth = 150;
        gameTime = 0.0f;
        score = 0;
        wave = 1;
        level = 1;
        xp = 0;
        xpToNextLevel = 60;
        damageMultiplier = 1.0f;
        fireRateMultiplier = 1.0f;
        hasRicochetUpgrade = false;
        hasPierceUpgrade = false;
        bulletSizeMultiplier = 1.0f;
        hasDeathExplosionUpgrade = false;
        explosionRadiusMultiplier = 1.0f;
        explosionChancePercent = 5.0f;
        playerSpeedMultiplier = 1.0f;
        hasDashUpgrade = true;
        dashCooldown = 1.5f;
        dashDuration = 0.12f;
        dashLength = 150.0f;
        dashCharges = 2;
        maxDashCharges = 2;
        hasRicochetPlusPlusUpgrade = false;
        ricochetExtraBounces = 0;
        piercePlusCount = 0;
        shotgunAmmo = 0;
        maxShotgunAmmo = 7;
        orbMagnetismMultiplier = 1.0f;
        shotgunPelletCountBonus = 0;
        shotgunPelletDamageMultiplier = 1.0f;
        hasAutoAttackerUpgrade = false;
        autoAttackerRange = 200.0f;
        followGunSpeedMultiplier = 1.0f;
        companionCount = 0;
        companionDamageMultiplier = 1.0f;
        companionFireRateMultiplier = 1.0f;
        orbCompanionCount = 0;
        orbCompanionSpeed = 337.5f;
        orbCompanionMagnetismMultiplier = 1.0f;
        shotgunCompanionCount = 0;
    }

    int GetHealth() const { return playerHealth; }
    int GetMaxHealth() const { return maxHealth; }
    void TakeDamage(int amount) { playerHealth -= amount; if (playerHealth < 0) playerHealth = 0; }
    void AddHealth(int amount) { playerHealth += amount; if (playerHealth > maxHealth) playerHealth = maxHealth; }
    bool IsGameOver() const { return playerHealth <= 0; }

    float GetTime() const { return gameTime; }
    void AddTime(float dt) { gameTime += dt; }

    int GetScore() const { return score; }
    void AddScore(int points) { score += points; }

    int GetXpPerKill() const
    {
        float waveXpMult = std::pow(1.125f, (float)(wave - 1));
        int xp = (int)(5.0f * std::pow(1.25f, (float)(wave - 1)) * waveXpMult);
        return xp < 1 ? 1 : xp;
    }

    int GetLevel() const { return level; }
    int GetXp() const { return xp; }
    int GetXpToNextLevel() const { return xpToNextLevel; }
    void AddXp(int amount) { xp += amount; }
    void LevelUp()
    {
        xp -= xpToNextLevel;
        if (xp < 0) xp = 0;
        level++;
        if (level == 2) xpToNextLevel = 100;
        else if (level == 3) xpToNextLevel = 200;
        else if (level == 4) xpToNextLevel = 400;
        else if (level == 5) xpToNextLevel = 600;
        else if (level == 6) xpToNextLevel = 1000;
        else xpToNextLevel = 1000 + (level - 6) * 400;
    }

    float GetDamageMultiplier() const { return damageMultiplier; }
    float GetFireRateMultiplier() const { return fireRateMultiplier; }
    void AddMaxHealth(int amount) { maxHealth += amount; playerHealth += amount; if (playerHealth > maxHealth) playerHealth = maxHealth; }
    void AddDamageMultiplier(float amount) { damageMultiplier += amount; }
    void AddFireRateMultiplier(float amount) { fireRateMultiplier += amount; }

    bool HasRicochetUpgrade() const { return hasRicochetUpgrade; }
    bool HasPierceUpgrade() const { return hasPierceUpgrade; }
    void SetRicochetUpgrade(bool v) { hasRicochetUpgrade = v; }
    void SetPierceUpgrade(bool v) { hasPierceUpgrade = v; }
    bool HasRicochetPlusPlusUpgrade() const { return hasRicochetPlusPlusUpgrade; }
    void SetRicochetPlusPlusUpgrade(bool v) { hasRicochetPlusPlusUpgrade = v; }
    int GetRicochetBounceCount() const { return 2 + ricochetExtraBounces; }
    void AddRicochetExtraBounces(int amount) { ricochetExtraBounces += amount; }
    int GetPierceCount() const { return 2 + piercePlusCount; }
    float GetPierceDamageMultiplier() const { return 1.0f + piercePlusCount * 0.1f; }
    void AddPiercePlus() { piercePlusCount++; }

    float GetBulletSizeMultiplier() const { return bulletSizeMultiplier; }
    void AddBulletSizeMultiplier(float amount) { bulletSizeMultiplier += amount; }

    bool HasDeathExplosionUpgrade() const { return hasDeathExplosionUpgrade; }
    void SetDeathExplosionUpgrade(bool v) { hasDeathExplosionUpgrade = v; }
    float GetExplosionRadiusMultiplier() const { return explosionRadiusMultiplier; }
    float GetExplosionChancePercent() const { return explosionChancePercent; }
    void AddExplosionRadiusMultiplier(float amount) { explosionRadiusMultiplier += amount; }
    void AddExplosionChancePercent(float amount) { explosionChancePercent += amount; if (explosionChancePercent > 12.5f) explosionChancePercent = 12.5f; }

    float GetPlayerSpeedMultiplier() const { return playerSpeedMultiplier; }
    void AddPlayerSpeedMultiplier(float amount) { playerSpeedMultiplier += amount; }

    bool HasDashUpgrade() const { return hasDashUpgrade; }
    void SetDashUpgrade(bool v) { hasDashUpgrade = v; }
    float GetDashCooldown() const { return dashCooldown; }
    float GetDashDuration() const { return dashDuration; }
    float GetDashLength() const { return dashLength; }
    void ReduceDashCooldown(float amount) { dashCooldown -= amount; if (dashCooldown < 0.5f) dashCooldown = 0.5f; }
    void AddDashLength(float amount) { dashLength += amount; }

    int GetDashCharges() const { return dashCharges; }
    int GetMaxDashCharges() const { return maxDashCharges; }
    bool ConsumeDashCharge() { if (dashCharges <= 0) return false; dashCharges--; return true; }
    void AddDashCharge() { if (dashCharges < maxDashCharges) dashCharges++; }
    void AddMaxDashCharges() { if (maxDashCharges < 4) { maxDashCharges++; dashCharges = maxDashCharges; } }

    int GetShotgunAmmo() const { return shotgunAmmo; }
    int GetMaxShotgunAmmo() const { return maxShotgunAmmo; }
    void AddShotgunAmmo(int amount) { shotgunAmmo += amount; if (shotgunAmmo > maxShotgunAmmo) shotgunAmmo = maxShotgunAmmo; }
    bool ConsumeShotgunAmmo(int amount) { if (shotgunAmmo < amount) return false; shotgunAmmo -= amount; return true; }

    int GetShotgunPelletCount() const { return 10 + shotgunPelletCountBonus; }
    int GetShotgunPelletUpgradeCount() const { return shotgunPelletCountBonus / 5; }
    float GetShotgunPelletDamageMultiplier() const { return shotgunPelletDamageMultiplier; }
    void AddShotgunPelletCount() { shotgunPelletCountBonus += 5; if (shotgunPelletCountBonus > 15) shotgunPelletCountBonus = 15; }
    void AddShotgunPelletDamageMultiplier(float amount) { shotgunPelletDamageMultiplier += amount; }

    float GetOrbMagnetismMultiplier() const { return orbMagnetismMultiplier; }
    void AddOrbMagnetismMultiplier(float amount) { orbMagnetismMultiplier += amount; }

    bool HasAutoAttackerUpgrade() const { return hasAutoAttackerUpgrade; }
    void SetAutoAttackerUpgrade(bool v) { hasAutoAttackerUpgrade = v; }
    float GetAutoAttackerRange() const { return autoAttackerRange; }
    void AddAutoAttackerRange(float amount) { autoAttackerRange += amount; }

    float GetFollowGunSpeedMultiplier() const { return followGunSpeedMultiplier; }
    void AddFollowGunSpeedMultiplier(float amount) { followGunSpeedMultiplier += amount; }

    int GetWave() const { return wave; }
    void NextWave() { wave++; }

    bool HasCompanionUpgrade() const { return companionCount >= 1; }
    int GetCompanionCount() const { return companionCount; }
    void SetCompanionUpgrade(bool v) { if (v && companionCount < 2) companionCount = 1; }
    void AddCompanion() { if (companionCount < 2) companionCount++; }
    float GetCompanionDamageMultiplier() const { return companionDamageMultiplier; }
    float GetCompanionFireRateMultiplier() const { return companionFireRateMultiplier; }
    void AddCompanionDamageMultiplier(float amount) { companionDamageMultiplier += amount; }
    void AddCompanionFireRateMultiplier(float amount) { companionFireRateMultiplier += amount; }

    bool HasOrbCompanionUpgrade() const { return orbCompanionCount >= 1; }
    int GetOrbCompanionCount() const { return orbCompanionCount; }
    void SetOrbCompanionUpgrade(bool v) { if (v && orbCompanionCount < 2) orbCompanionCount = 1; }
    void AddOrbCompanion() { if (orbCompanionCount < 2) orbCompanionCount++; }
    float GetOrbCompanionSpeed() const { return orbCompanionSpeed; }
    float GetOrbCompanionMagnetismMultiplier() const { return orbCompanionMagnetismMultiplier; }
    void AddOrbCompanionSpeed(float amount) { orbCompanionSpeed += amount; }
    void AddOrbCompanionMagnetismMultiplier(float amount) { orbCompanionMagnetismMultiplier += amount; }

    bool HasShotgunCompanionUpgrade() const { return shotgunCompanionCount >= 1; }
    int GetShotgunCompanionCount() const { return shotgunCompanionCount; }
    void SetShotgunCompanionUpgrade(bool v) { if (v && shotgunCompanionCount < 2) shotgunCompanionCount = 1; }
    void AddShotgunCompanion() { if (shotgunCompanionCount < 2) shotgunCompanionCount++; }
};
