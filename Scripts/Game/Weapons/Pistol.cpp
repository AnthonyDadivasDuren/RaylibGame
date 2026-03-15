#include "Game/Weapons/Pistol.h"
#include "Pools/EnemyPool.h"
#include "Pools/BulletPool.h"
#include "Core/GameManager.h"
#include <cmath>

namespace Pistol
{
void Fire(float px, float py, float lastMoveX, float lastMoveY, EnemyPool* enemyPool, BulletPool* bulletPool, GameManager* gm,
          bool bossActive, float bossX, float bossY)
{
    if (!bulletPool || !gm) return;

    float bdx = lastMoveX;
    float bdy = lastMoveY;
    float bestDistSq = 999999.0f;
    float targetX = px + lastMoveX;
    float targetY = py + lastMoveY;

    if (enemyPool)
    {
        Enemy* closestInDirection = nullptr;
        for (int i = 0; i < enemyPool->GetPoolSize(); i++)
        {
            Enemy* e = enemyPool->GetEnemy(i);
            if (!e || !e->active) continue;
            float toEx = e->position.x - px;
            float toEy = e->position.y - py;
            float distSq = toEx * toEx + toEy * toEy;
            if (distSq < 1.0f) continue;
            float dot = toEx * lastMoveX + toEy * lastMoveY;
            if (dot > 0.0f && distSq < bestDistSq)
            {
                bestDistSq = distSq;
                closestInDirection = e;
            }
        }
        if (closestInDirection)
        {
            targetX = closestInDirection->position.x;
            targetY = closestInDirection->position.y;
        }
    }
    if (bossActive)
    {
        float toBossX = bossX - px;
        float toBossY = bossY - py;
        float distSq = toBossX * toBossX + toBossY * toBossY;
        if (distSq > 1.0f)
        {
            float dot = toBossX * lastMoveX + toBossY * lastMoveY;
            if (dot > 0.0f && distSq < bestDistSq)
            {
                targetX = bossX;
                targetY = bossY;
                bestDistSq = distSq;
            }
        }
    }

    bdx = targetX - px;
    bdy = targetY - py;

    float dmgMult = gm->GetDamageMultiplier();
    if (gm->HasPierceUpgrade()) dmgMult *= gm->GetPierceDamageMultiplier();
    int damage = (int)(14 * dmgMult);
    if (damage < 1) damage = 1;
    float len = std::sqrt(bdx * bdx + bdy * bdy);
    if (len < 0.001f) { bdx = 1.0f; bdy = 0.0f; }
    else { bdx /= len; bdy /= len; }

    const float bulletSpeed = 400.0f * gm->GetFollowGunSpeedMultiplier();
    int hitsLeft = 1;
    bool isRicochet = false;
    bool ricochetPP = false;
    if (gm->HasRicochetUpgrade())
    {
        hitsLeft = gm->GetRicochetBounceCount();
        isRicochet = true;
        ricochetPP = gm->HasRicochetPlusPlusUpgrade();
    }
    if (gm->HasPierceUpgrade() && gm->GetPierceCount() > hitsLeft)
        hitsLeft = gm->GetPierceCount();
    float bulletRadius = 10.0f * gm->GetBulletSizeMultiplier();
    bulletPool->Spawn(px, py, bdx * bulletSpeed, bdy * bulletSpeed, damage, hitsLeft, isRicochet, bulletRadius, ricochetPP, 5.0f);
}
}
