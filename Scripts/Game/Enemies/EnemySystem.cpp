#include "Game/Enemies/EnemySystem.h"
#include "Pools/EnemyPool.h"
#include "Pools/BulletPool.h"
#include "Core/GameManager.h"
#include "Game/Player/Player.h"
#include "Shared/EnemyFactory.h"
#include <cmath>

void EnemySystem::Update(float deltaTime, float playerX, float playerY, int wave, EnemyPool* enemyPool)
{
    if (!enemyPool) return;

    float speedMult = 0.8f + (wave - 1) * 0.1f;
    if (speedMult > 1.5f) speedMult = 1.5f;

    for (int i = 0; i < enemyPool->GetPoolSize(); i++)
    {
        Enemy* e = enemyPool->GetEnemy(i);
        if (!e || !e->active) continue;
        float edx = playerX - e->position.x;
        float edy = playerY - e->position.y;
        float len = std::sqrt(edx * edx + edy * edy);
        if (len > 0.001f)
        {
            edx /= len;
            edy /= len;
            EnemyFactory::EnemyConfig cfg = EnemyFactory::GetConfig(e->type);
            e->velocity.x = edx * cfg.speed * speedMult;
            e->velocity.y = edy * cfg.speed * speedMult;
        }
    }

    enemyPool->UpdateAll(deltaTime);
    enemyPool->ResolveEnemyOverlaps();
}

void EnemySystem::HandlePlayerCollisions(Player* player, EnemyPool* enemyPool, BulletPool* bulletPool, GameManager* gm, float playerX, float playerY)
{
    if (!player || !gm) return;

    if (bulletPool && player->IsVulnerable())
    {
        float playerR = player->GetSize() * 0.5f;
        for (int j = 0; j < bulletPool->GetPoolSize(); j++)
        {
            Bullet* b = bulletPool->GetBullet(j);
            if (!b || !b->active || !b->damagesPlayer) continue;
            float dx = b->position.x - playerX, dy = b->position.y - playerY;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < playerR + b->radius)
            {
                gm->TakeDamage(b->damage);
                player->SetHitFlashTimer(0.2f);
                player->SetHitInvincibleTimer(0.2f);
                if (dist > 0.001f) { dx /= dist; dy /= dist; }
                else { dx = 1.0f; dy = 0.0f; }
                player->ApplyKnockback(dx * 220.0f, dy * 220.0f);
                b->Deactivate();
            }
        }
    }

    if (enemyPool)
    {
        for (int i = 0; i < enemyPool->GetPoolSize(); i++)
        {
            Enemy* e = enemyPool->GetEnemy(i);
            if (!e || !e->active) continue;
            float ex = e->position.x, ey = e->position.y;
            float dist = std::sqrt((playerX - ex) * (playerX - ex) + (playerY - ey) * (playerY - ey));
            float minDist = player->GetSize() * 0.5f + e->radius;
            if (dist < minDist && player->IsVulnerable())
            {
                gm->TakeDamage(10);
                player->SetHitFlashTimer(0.2f);
                player->SetHitInvincibleTimer(0.2f);
                float pdx = playerX - ex, pdy = playerY - ey;
                float len = std::sqrt(pdx * pdx + pdy * pdy);
                if (len > 0.001f)
                {
                    pdx /= len;
                    pdy /= len;
                    player->ApplyKnockback(pdx * 220.0f, pdy * 220.0f);
                    e->position.x = playerX + (ex - playerX) / len * minDist * 1.1f;
                    e->position.y = playerY + (ey - playerY) / len * minDist * 1.1f;
                }
                else
                    player->ApplyKnockback(220.0f, 0.0f);
            }
        }
    }
}
