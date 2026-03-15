#include "Game/Weapons/Shotgun.h"
#include "Pools/BulletPool.h"
#include "Pools/ParticlePool.h"
#include "Core/GameManager.h"
#include "Core/Viewport.h"
#include "raylib.h"
#include <cmath>

namespace Shotgun
{
void Fire(float px, float py, BulletPool* bulletPool, ParticlePool* particlePool, GameManager* gm)
{
    if (!bulletPool || !gm) return;

    float aimX = Viewport::GetGameMouseX() - px;
    float aimY = Viewport::GetGameMouseY() - py;
    float aimLen = std::sqrt(aimX * aimX + aimY * aimY);
    if (aimLen < 0.001f) { aimX = 1.0f; aimY = 0.0f; }
    else { aimX /= aimLen; aimY /= aimLen; }

    if (particlePool) particlePool->SpawnMuzzleFlash(px, py, aimX, aimY, 52);

    int pelletCount = gm->GetShotgunPelletCount();
    float totalDamage = 135.0f * (float)pelletCount / 10.0f * gm->GetDamageMultiplier() * gm->GetShotgunPelletDamageMultiplier();
    int pelletDamage = (int)(totalDamage / (float)pelletCount);
    if (pelletDamage < 1) pelletDamage = 1;
    const float bulletSpeed = 420.0f;
    const float pelletRadius = 6.5f;
    float spreadRad = 0.22f + (float)(pelletCount - 10) * 0.008f;
    if (spreadRad < 0.22f) spreadRad = 0.22f;
    const float shotgunKnockback = 10.0f;

    for (int p = 0; p < pelletCount; p++)
    {
        float angle = (float)GetRandomValue(-100, 100) / 100.0f * spreadRad;
        float c = std::cos(angle), s = std::sin(angle);
        float bdx = aimX * c - aimY * s;
        float bdy = aimX * s + aimY * c;
        bulletPool->Spawn(px, py, bdx * bulletSpeed, bdy * bulletSpeed, pelletDamage, 2, false, pelletRadius, false, shotgunKnockback, true);
    }
}
}
