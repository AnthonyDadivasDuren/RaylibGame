#include "Game/Weapons/WeaponSystem.h"
#include "Game/Weapons/Pistol.h"
#include "Game/Weapons/Shotgun.h"
#include "Core/GameManager.h"
#include "Pools/EnemyPool.h"
#include "Pools/BulletPool.h"
#include "raylib.h"
#include <cmath>

WeaponSystem::WeaponSystem() : currentWeapon_(0), fireCooldown_(0.0f), fireIntervalFollow_(0.533333f), autoAttackerCooldown_(0.0f)
{
}

bool WeaponSystem::TryFirePistol(float deltaTime, float px, float py, float lastMoveX, float lastMoveY, EnemyPool* enemyPool, BulletPool* bulletPool, GameManager* gm,
                                bool bossActive, float bossX, float bossY)
{
    if (currentWeapon_ != 0 || !gm) return false;
    fireCooldown_ -= deltaTime;
    if (fireCooldown_ > 0.0f || !bulletPool) return false;
    float effectiveInterval = fireIntervalFollow_ / gm->GetFireRateMultiplier();
    fireCooldown_ = effectiveInterval;
    Pistol::Fire(px, py, lastMoveX, lastMoveY, enemyPool, bulletPool, gm, bossActive, bossX, bossY);
    return true;
}

bool WeaponSystem::TryFireShotgun(float px, float py, BulletPool* bulletPool, ParticlePool* particlePool, GameManager* gm)
{
    if (currentWeapon_ != 1 || !gm) return false;
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return false;
    if (!gm->ConsumeShotgunAmmo(1)) return false;
    Shotgun::Fire(px, py, bulletPool, particlePool, gm);
    return true;
}

bool WeaponSystem::TryFireAutoAttacker(float deltaTime, float px, float py, EnemyPool* enemyPool, BulletPool* bulletPool,
                                       bool bossActive, float bossX, float bossY, GameManager* gm)
{
    if (!gm || !gm->HasAutoAttackerUpgrade() || !enemyPool || !bulletPool) return false;
    autoAttackerCooldown_ -= deltaTime;
    if (autoAttackerCooldown_ > 0.0f) return false;

    float range = gm->GetAutoAttackerRange();
    float rangeSq = range * range;
    Enemy* closest = nullptr;
    float bestDistSq = rangeSq;
    for (int i = 0; i < enemyPool->GetPoolSize(); i++)
    {
        Enemy* e = enemyPool->GetEnemy(i);
        if (!e || !e->active) continue;
        float dx = e->position.x - px, dy = e->position.y - py;
        float dsq = dx * dx + dy * dy;
        if (dsq < bestDistSq && dsq > 1.0f) { bestDistSq = dsq; closest = e; }
    }
    float targetX = 0.0f, targetY = 0.0f;
    bool haveTarget = false;
    if (closest)
    {
        targetX = closest->position.x;
        targetY = closest->position.y;
        haveTarget = true;
    }
    if (bossActive)
    {
        float bdx = bossX - px, bdy = bossY - py;
        float dsq = bdx * bdx + bdy * bdy;
        if (dsq > 1.0f && dsq < rangeSq && (!haveTarget || dsq < bestDistSq))
        {
            targetX = bossX;
            targetY = bossY;
            haveTarget = true;
            bestDistSq = dsq;
        }
    }
    if (!haveTarget) return false;

    float bdx = targetX - px, bdy = targetY - py;
    float len = std::sqrt(bdx * bdx + bdy * bdy);
    if (len > 0.001f) { bdx /= len; bdy /= len; }
    int damage = (int)(14.0f * gm->GetDamageMultiplier());
    if (damage < 1) damage = 1;
    const float bulletSpeed = 400.0f;
    float bulletRadius = 10.0f * gm->GetBulletSizeMultiplier();
    bulletPool->Spawn(px, py, bdx * bulletSpeed, bdy * bulletSpeed, damage, 1, false, bulletRadius, false, 5.0f);
    float effectiveInterval = fireIntervalFollow_ / gm->GetFireRateMultiplier();
    autoAttackerCooldown_ = effectiveInterval;
    return true;
}

void WeaponSystem::Reset()
{
    currentWeapon_ = 0;
    fireCooldown_ = 0.0f;
    autoAttackerCooldown_ = 0.0f;
}
