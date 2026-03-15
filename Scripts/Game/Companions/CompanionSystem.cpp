#include "Game/Companions/CompanionSystem.h"
#include "Core/GameManager.h"
#include "Pools/EnemyPool.h"
#include "Pools/BulletPool.h"
#include "Pools/XpOrbPool.h"
#include "raylib.h"
#include <cmath>

CompanionSystem::CompanionSystem()
{
    for (int i = 0; i < MaxCompanions; i++)
    {
        pistolX_[i] = pistolY_[i] = 0.0f;
        pistolFireCooldown_[i] = 0.0f;
        orbX_[i] = orbY_[i] = 0.0f;
        shotgunX_[i] = shotgunY_[i] = 0.0f;
        shotgunFireCooldown_[i] = 0.0f;
    }
}

void CompanionSystem::Update(float deltaTime, float playerX, float playerY, float playerLastMoveX, float playerLastMoveY,
                             int screenWidth, int screenHeight,
                             EnemyPool* enemyPool, BulletPool* bulletPool, XpOrbPool* xpOrbPool, GameManager* gm)
{
    if (!gm) return;

    int nPistol = gm->GetCompanionCount();
    for (int c = 0; c < nPistol; c++)
    {
        float offset = 50.0f + (float)c * 45.0f;
        float targetCx = playerX - playerLastMoveX * offset;
        float targetCy = playerY - playerLastMoveY * offset;
        float clen = std::sqrt(playerLastMoveX * playerLastMoveX + playerLastMoveY * playerLastMoveY);
        if (clen < 0.001f) { targetCx = playerX - offset; targetCy = playerY; }
        float cdx = targetCx - pistolX_[c], cdy = targetCy - pistolY_[c];
        float cdist = std::sqrt(cdx * cdx + cdy * cdy);
        if (cdist > 0.001f) { cdx /= cdist; cdy /= cdist; }
        float move = 320.0f * deltaTime;
        if (move > cdist) move = cdist;
        pistolX_[c] += cdx * move;
        pistolY_[c] += cdy * move;
        pistolFireCooldown_[c] -= deltaTime;
        if (pistolFireCooldown_[c] <= 0.0f && bulletPool && enemyPool)
        {
            float rangeSq = 400.0f * 400.0f;
            Enemy* closest = nullptr;
            float bestDistSq = rangeSq;
            for (int i = 0; i < enemyPool->GetPoolSize(); i++)
            {
                Enemy* e = enemyPool->GetEnemy(i);
                if (!e || !e->active) continue;
                float dx = e->position.x - pistolX_[c], dy = e->position.y - pistolY_[c];
                float dsq = dx * dx + dy * dy;
                if (dsq < bestDistSq && dsq > 1.0f) { bestDistSq = dsq; closest = e; }
            }
            if (closest)
            {
                float bdx = closest->position.x - pistolX_[c], bdy = closest->position.y - pistolY_[c];
                float len = std::sqrt(bdx * bdx + bdy * bdy);
                if (len > 0.001f) { bdx /= len; bdy /= len; }
                int dmg = (int)(10.0f * gm->GetCompanionDamageMultiplier());
                if (dmg < 1) dmg = 1;
                float interval = 0.6f / gm->GetCompanionFireRateMultiplier();
                pistolFireCooldown_[c] = interval;
                bulletPool->Spawn(pistolX_[c], pistolY_[c], bdx * 380.0f, bdy * 380.0f, dmg, 1, false, 8.0f, false, 4.0f);
            }
        }
    }

    int nOrb = gm->GetOrbCompanionCount();
    if (nOrb > 0 && xpOrbPool)
    {
        float orbSpeed = gm->GetOrbCompanionSpeed() * deltaTime;
        float collectRad = 22.0f + 4.0f;
        float maxTargetDistSq = (float)(screenWidth * screenWidth + screenHeight * screenHeight) * 2.0f;
        for (int c = 0; c < nOrb; c++)
        {
            int nearestIdx = -1;
            float nearestDistSq = maxTargetDistSq;
            for (int i = 0; i < xpOrbPool->GetPoolSize(); i++)
            {
                XpOrb* orb = xpOrbPool->GetOrb(i);
                if (!orb || !orb->active) continue;
                float dx = orb->position.x - orbX_[c], dy = orb->position.y - orbY_[c];
                float dsq = dx * dx + dy * dy;
                if (dsq < nearestDistSq) { nearestDistSq = dsq; nearestIdx = i; }
            }
            if (nearestIdx >= 0)
            {
                XpOrb* orb = xpOrbPool->GetOrb(nearestIdx);
                float dx = orb->position.x - orbX_[c], dy = orb->position.y - orbY_[c];
                float dist = std::sqrt(dx * dx + dy * dy);
                if (dist > 0.001f) { dx /= dist; dy /= dist; }
                float move = (orbSpeed < dist) ? orbSpeed : dist;
                orbX_[c] += dx * move;
                orbY_[c] += dy * move;
            }
            else
            {
                float dx = playerX - orbX_[c], dy = playerY - orbY_[c];
                float dist = std::sqrt(dx * dx + dy * dy);
                if (dist > 0.001f) { dx /= dist; dy /= dist; }
                float move = (orbSpeed < dist) ? orbSpeed : dist;
                orbX_[c] += dx * move;
                orbY_[c] += dy * move;
            }
            for (int i = 0; i < xpOrbPool->GetPoolSize(); i++)
            {
                XpOrb* orb = xpOrbPool->GetOrb(i);
                if (!orb || !orb->active) continue;
                float dx = orb->position.x - orbX_[c], dy = orb->position.y - orbY_[c];
                float dist = std::sqrt(dx * dx + dy * dy);
                if (dist < collectRad)
                {
                    gm->AddXp(orb->xpValue);
                    orb->Deactivate();
                }
            }
        }
    }

    int nShotgun = gm->GetShotgunCompanionCount();
    for (int c = 0; c < nShotgun; c++)
    {
        float offset = 50.0f + (float)(nPistol + c) * 45.0f;
        float targetSx = playerX - playerLastMoveX * offset;
        float targetSy = playerY - playerLastMoveY * offset;
        float slen = std::sqrt(playerLastMoveX * playerLastMoveX + playerLastMoveY * playerLastMoveY);
        if (slen < 0.001f) { targetSx = playerX - offset; targetSy = playerY; }
        float sdx = targetSx - shotgunX_[c], sdy = targetSy - shotgunY_[c];
        float sdist = std::sqrt(sdx * sdx + sdy * sdy);
        if (sdist > 0.001f) { sdx /= sdist; sdy /= sdist; }
        float smove = 320.0f * deltaTime;
        if (smove > sdist) smove = sdist;
        shotgunX_[c] += sdx * smove;
        shotgunY_[c] += sdy * smove;
        shotgunFireCooldown_[c] -= deltaTime;
        if (shotgunFireCooldown_[c] <= 0.0f && bulletPool && enemyPool)
        {
            float rangeSq = 380.0f * 380.0f;
            Enemy* closest = nullptr;
            float bestDistSq = rangeSq;
            for (int i = 0; i < enemyPool->GetPoolSize(); i++)
            {
                Enemy* e = enemyPool->GetEnemy(i);
                if (!e || !e->active) continue;
                float dx = e->position.x - shotgunX_[c], dy = e->position.y - shotgunY_[c];
                float dsq = dx * dx + dy * dy;
                if (dsq < bestDistSq && dsq > 1.0f) { bestDistSq = dsq; closest = e; }
            }
            float aimX = 1.0f, aimY = 0.0f;
            if (closest)
            {
                aimX = closest->position.x - shotgunX_[c];
                aimY = closest->position.y - shotgunY_[c];
                float alen = std::sqrt(aimX * aimX + aimY * aimY);
                if (alen > 0.001f) { aimX /= alen; aimY /= alen; }
            }
            const int pelletCount = 5;
            const float pelletDamage = 12.0f;
            const float bulletSpeed = 380.0f;
            const float pelletRadius = 5.0f;
            const float spreadRad = 0.28f;
            const float shotgunKnockback = 5.0f;
            for (int p = 0; p < pelletCount; p++)
            {
                float angle = (float)GetRandomValue(-100, 100) / 100.0f * spreadRad;
                float co = std::cos(angle), si = std::sin(angle);
                float bdx = aimX * co - aimY * si;
                float bdy = aimX * si + aimY * co;
                bulletPool->Spawn(shotgunX_[c], shotgunY_[c], bdx * bulletSpeed, bdy * bulletSpeed, (int)pelletDamage, 2, false, pelletRadius, false, shotgunKnockback, true);
            }
            shotgunFireCooldown_[c] = 1.2f;
        }
    }

    ResolveOverlaps(gm);
}

void CompanionSystem::ResolveOverlaps(GameManager* gm)
{
    float* cx[6];
    float* cy[6];
    float cr[6];
    int nc = 0;
    for (int i = 0; i < gm->GetCompanionCount(); i++)
    { cx[nc] = &pistolX_[i]; cy[nc] = &pistolY_[i]; cr[nc] = 12.5f; nc++; }
    for (int i = 0; i < gm->GetOrbCompanionCount(); i++)
    { cx[nc] = &orbX_[i]; cy[nc] = &orbY_[i]; cr[nc] = 11.0f; nc++; }
    for (int i = 0; i < gm->GetShotgunCompanionCount(); i++)
    { cx[nc] = &shotgunX_[i]; cy[nc] = &shotgunY_[i]; cr[nc] = 12.5f; nc++; }
    for (int iter = 0; iter < 3; iter++)
        for (int i = 0; i < nc; i++)
            for (int j = i + 1; j < nc; j++)
            {
                float dx = *cx[j] - *cx[i], dy = *cy[j] - *cy[i];
                float dist = std::sqrt(dx * dx + dy * dy);
                float sumR = cr[i] + cr[j];
                if (dist < sumR && dist > 0.001f)
                {
                    float push = (sumR - dist) * 0.5f;
                    dx /= dist; dy /= dist;
                    *cx[i] -= dx * push; *cy[i] -= dy * push;
                    *cx[j] += dx * push; *cy[j] += dy * push;
                }
            }
}

float CompanionSystem::GetPistolX(int index) const { return (index >= 0 && index < MaxCompanions) ? pistolX_[index] : 0.0f; }
float CompanionSystem::GetPistolY(int index) const { return (index >= 0 && index < MaxCompanions) ? pistolY_[index] : 0.0f; }
float CompanionSystem::GetOrbX(int index) const { return (index >= 0 && index < MaxCompanions) ? orbX_[index] : 0.0f; }
float CompanionSystem::GetOrbY(int index) const { return (index >= 0 && index < MaxCompanions) ? orbY_[index] : 0.0f; }
float CompanionSystem::GetShotgunX(int index) const { return (index >= 0 && index < MaxCompanions) ? shotgunX_[index] : 0.0f; }
float CompanionSystem::GetShotgunY(int index) const { return (index >= 0 && index < MaxCompanions) ? shotgunY_[index] : 0.0f; }

void CompanionSystem::Reset(float startX, float startY)
{
    for (int i = 0; i < MaxCompanions; i++)
    {
        pistolX_[i] = startX; pistolY_[i] = startY;
        pistolFireCooldown_[i] = 0.0f;
        orbX_[i] = startX; orbY_[i] = startY;
        shotgunX_[i] = startX; shotgunY_[i] = startY;
        shotgunFireCooldown_[i] = 0.0f;
    }
}

void CompanionSystem::Draw() const
{
    const GameManager& gm = GameManager::Instance();
    for (int c = 0; c < gm.GetCompanionCount(); c++)
    {
        float cs = 25.0f;
        DrawRectangle((int)(GetPistolX(c) - cs * 0.5f), (int)(GetPistolY(c) - cs * 0.5f), (int)cs, (int)cs, SKYBLUE);
    }
    for (int c = 0; c < gm.GetOrbCompanionCount(); c++)
    {
        float ocs = 22.0f;
        DrawRectangle((int)(GetOrbX(c) - ocs * 0.5f), (int)(GetOrbY(c) - ocs * 0.5f), (int)ocs, (int)ocs, LIME);
    }
    for (int c = 0; c < gm.GetShotgunCompanionCount(); c++)
    {
        float scs = 25.0f;
        DrawRectangle((int)(GetShotgunX(c) - scs * 0.5f), (int)(GetShotgunY(c) - scs * 0.5f), (int)scs, (int)scs, ORANGE);
    }
}
