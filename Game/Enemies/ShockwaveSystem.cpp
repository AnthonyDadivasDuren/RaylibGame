#include "ShockwaveSystem.h"
#include "../../EnemyPool.h"
#include "../../ParticlePool.h"
#include "../../XpOrbPool.h"
#include "../../GameManager.h"
#include "raylib.h"
#include <cmath>
#include <algorithm>

void ShockwaveSystem::Draw() const
{
    for (const ExpandingShockwave& sw : shockwaves_)
    {
        float t = sw.currentRadius / sw.maxRadius;
        float alpha = 0.85f - t * 0.5f;
        if (alpha < 0.2f) alpha = 0.2f;
        DrawCircleLines((int)sw.x, (int)sw.y, sw.currentRadius, ColorAlpha(LIGHTGRAY, alpha));
        DrawCircleLines((int)sw.x, (int)sw.y, sw.currentRadius - 1.0f, ColorAlpha(WHITE, alpha * 0.6f));
    }
}

void ShockwaveSystem::AddShockwave(const ExpandingShockwave& sw)
{
    shockwaves_.push_back(sw);
}

void ShockwaveSystem::Update(float deltaTime, EnemyPool* enemyPool, ParticlePool* particlePool, XpOrbPool* xpOrbPool, GameManager* gm)
{
    for (size_t s = 0; s < shockwaves_.size(); s++)
    {
        ExpandingShockwave& sw = shockwaves_[s];
        sw.currentRadius += sw.expansionSpeed * deltaTime;
        if (!enemyPool || sw.currentRadius <= 0.0f) continue;
        float damageScale = 10.0f + (sw.currentRadius / sw.maxRadius) * 25.0f;
        float knockbackStrength = 150.0f + (sw.currentRadius / sw.maxRadius) * 150.0f;
        for (int k = 0; k < enemyPool->GetPoolSize(); k++)
        {
            if (k == sw.excludeEnemyIndex || sw.hitIndices.count(k)) continue;
            Enemy* e = enemyPool->GetEnemy(k);
            if (!e || !e->active) continue;
            float dx = e->position.x - sw.x, dy = e->position.y - sw.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist > sw.currentRadius) continue;
            sw.hitIndices.insert(k);
            float kx = e->position.x, ky = e->position.y;
            Color kc = e->color;
            int xpMult = e->xpMultiplier;
            EnemyType etype = e->type;
            if (dist > 0.001f) { dx /= dist; dy /= dist; }
            e->ApplyKnockback(dx, dy, knockbackStrength);
            e->TakeDamage((int)damageScale);
            if (!e->active)
            {
                if (xpOrbPool && gm) xpOrbPool->SpawnMany(kx, ky, gm->GetXpPerKill() * xpMult);
                if (particlePool) particlePool->SpawnBloodSplatter(kx, ky, kc, 36, dx, dy);
                if (etype == EnemyType::SPLITTER)
                {
                    const float splitOffset = 28.0f;
                    enemyPool->Spawn(EnemyType::SPLITTER_SMALL, kx - splitOffset, ky);
                    enemyPool->Spawn(EnemyType::SPLITTER_SMALL, kx + splitOffset, ky);
                    enemyPool->Spawn(EnemyType::SPLITTER_SMALL, kx, ky - splitOffset);
                    enemyPool->Spawn(EnemyType::SPLITTER_SMALL, kx, ky + splitOffset);
                }
            }
        }
    }
    shockwaves_.erase(std::remove_if(shockwaves_.begin(), shockwaves_.end(), [](const ExpandingShockwave& sw) { return sw.currentRadius >= sw.maxRadius; }), shockwaves_.end());
}
