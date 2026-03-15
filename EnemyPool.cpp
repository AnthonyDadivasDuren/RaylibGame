#include "EnemyPool.h"
#include "raylib.h"
#include <cmath>

void Enemy::Activate(float x, float y, EnemyType enemyType)
{
    type = enemyType;
    EnemyFactory::EnemyConfig config = EnemyFactory::GetConfig(type);
    position = { x, y };
    velocity = { 0, 0 };
    health = config.health;
    maxHealth = config.health;
    radius = config.radius;
    color = config.color;
    active = true;
    hitFlashTimer = 0.0f;
    knockbackVelocity = { 0, 0 };
    isSpecial = false;
    xpMultiplier = (type == EnemyType::ARMORED) ? 10 : 1;
    hasArmor = (type == EnemyType::ARMORED);
}

void Enemy::Deactivate()
{
    active = false;
}

void Enemy::ApplyKnockback(float vx, float vy, float strength)
{
    float len = std::sqrt(vx * vx + vy * vy);
    if (len < 0.001f) return;
    knockbackVelocity.x += (vx / len) * strength;
    knockbackVelocity.y += (vy / len) * strength;
}

void Enemy::Update(float deltaTime)
{
    if (!active) return;
    position.x += (velocity.x + knockbackVelocity.x) * deltaTime;
    position.y += (velocity.y + knockbackVelocity.y) * deltaTime;
    knockbackVelocity.x *= 0.85f;
    knockbackVelocity.y *= 0.85f;

    if (hitFlashTimer > 0.0f)
    {
        hitFlashTimer -= deltaTime;
        if (hitFlashTimer < 0.0f) hitFlashTimer = 0.0f;
    }
}

void Enemy::TakeDamage(int amount)
{
    health -= amount;
    hitFlashTimer = 0.2f;
    if (health <= 0) Deactivate();
}

void Enemy::Draw() const
{
    if (!active) return;
    Color drawColor = color;
    if (type == EnemyType::ARMORED && !hasArmor)
        drawColor = GRAY;
    else if (isSpecial)
    {
        float t = (sinf(GameManager::Instance().GetTime() * 4.0f) + 1.0f) * 0.5f;
        Color a = GOLD, b = MAGENTA;
        drawColor.r = (unsigned char)(a.r + (float)(b.r - a.r) * t);
        drawColor.g = (unsigned char)(a.g + (float)(b.g - a.g) * t);
        drawColor.b = (unsigned char)(a.b + (float)(b.b - a.b) * t);
    }
    float intensity = 0.0f;
    if (hitFlashTimer > 0.0f)
    {
        const float total = 0.2f;
        if (hitFlashTimer > total - 0.05f)
            intensity = (total - hitFlashTimer) / 0.05f;
        else if (hitFlashTimer < 0.05f)
            intensity = hitFlashTimer / 0.05f;
        else
            intensity = 1.0f;
    }
    if (intensity > 0.001f)
    {
        drawColor.r = (unsigned char)(drawColor.r + (255 - drawColor.r) * intensity);
        drawColor.g = (unsigned char)(drawColor.g + (255 - drawColor.g) * intensity);
        drawColor.b = (unsigned char)(drawColor.b + (255 - drawColor.b) * intensity);
    }
    if (type == EnemyType::ARMORED && hasArmor)
    {
        const float armorThickness = 10.0f;
        float armorRadius = radius + armorThickness;
        DrawCircleLines((int)position.x, (int)position.y, armorRadius, ColorAlpha(DARKGRAY, 0.9f));
        DrawCircleLines((int)position.x, (int)position.y, armorRadius - 3.0f, ColorAlpha(LIGHTGRAY, 0.7f));
    }
    DrawCircleV(position, radius, drawColor);
    DrawCircleLines((int)position.x, (int)position.y, radius, ColorBrightness(drawColor, -0.3f));
}

Rectangle Enemy::GetBounds() const
{
    return { position.x - radius, position.y - radius, radius * 2, radius * 2 };
}

EnemyPool::EnemyPool(int size) : poolSize(size)
{
    pool.resize(size);
}

Enemy* EnemyPool::Spawn(EnemyType type, float x, float y)
{
    for (int i = 0; i < poolSize; i++)
    {
        if (!pool[i].active)
        {
            pool[i].Activate(x, y, type);
            if (GetRandomValue(1, 100) <= 2)
            {
                pool[i].isSpecial = true;
                pool[i].xpMultiplier = GetRandomValue(5, 15);
                pool[i].health *= 3;
                pool[i].maxHealth *= 3;
            }
            return &pool[i];
        }
    }
    return nullptr;
}

void EnemyPool::UpdateAll(float deltaTime)
{
    for (int i = 0; i < poolSize; i++)
        pool[i].Update(deltaTime);
}

void EnemyPool::ResolveEnemyOverlaps()
{
    for (int i = 0; i < poolSize; i++)
    {
        if (!pool[i].active) continue;
        for (int j = i + 1; j < poolSize; j++)
        {
            if (!pool[j].active) continue;
            float dx = pool[j].position.x - pool[i].position.x;
            float dy = pool[j].position.y - pool[i].position.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            float minDist = pool[i].radius + pool[j].radius;
            if (dist < minDist && dist > 0.001f)
            {
                float overlap = minDist - dist;
                dx /= dist;
                dy /= dist;
                pool[i].position.x -= dx * overlap * 0.5f;
                pool[i].position.y -= dy * overlap * 0.5f;
                pool[j].position.x += dx * overlap * 0.5f;
                pool[j].position.y += dy * overlap * 0.5f;
            }
        }
    }
}

void EnemyPool::DrawAll() const
{
    for (int i = 0; i < poolSize; i++)
        pool[i].Draw();
}

Enemy* EnemyPool::GetEnemy(int index)
{
    if (index >= 0 && index < poolSize) return &pool[index];
    return nullptr;
}

int EnemyPool::GetPoolSize() const
{
    return poolSize;
}

int EnemyPool::GetActiveCount() const
{
    int count = 0;
    for (int i = 0; i < poolSize; i++)
        if (pool[i].active) count++;
    return count;
}

void EnemyPool::ResetPool()
{
    for (int i = 0; i < poolSize; i++)
        pool[i].Deactivate();
}

void EnemyPool::ShockwaveAt(float x, float y, float radius, int damage, int excludeIndex, ParticlePool* particlePool, XpOrbPool* xpOrbPool)
{
    for (int k = 0; k < poolSize; k++)
    {
        if (k == excludeIndex || !pool[k].active) continue;
        float dx = pool[k].position.x - x;
        float dy = pool[k].position.y - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist >= radius || dist < 0.001f) continue;
        float kx = pool[k].position.x, ky = pool[k].position.y;
        Color kc = pool[k].color;
        pool[k].TakeDamage(damage);
        if (!pool[k].active)
        {
            int xpAmount = GameManager::Instance().GetXpPerKill() * pool[k].xpMultiplier;
            if (xpOrbPool) xpOrbPool->SpawnMany(kx, ky, xpAmount);
            if (particlePool)
            {
                float ndx = dx / dist, ndy = dy / dist;
                particlePool->SpawnBloodSplatter(kx, ky, kc, 36, ndx, ndy);
            }
            if (pool[k].type == EnemyType::SPLITTER)
            {
                const float splitOffset = 28.0f;
                Spawn(EnemyType::SPLITTER_SMALL, kx - splitOffset, ky);
                Spawn(EnemyType::SPLITTER_SMALL, kx + splitOffset, ky);
                Spawn(EnemyType::SPLITTER_SMALL, kx, ky - splitOffset);
                Spawn(EnemyType::SPLITTER_SMALL, kx, ky + splitOffset);
            }
        }
    }
}

void EnemyPool::CheckCollisions(BulletPool& bulletPool, ParticlePool* particlePool, XpOrbPool* xpOrbPool, HealthOrbPool* healthOrbPool)
{
    for (int j = 0; j < bulletPool.GetPoolSize(); j++)
    {
        Bullet* b = bulletPool.GetBullet(j);
        if (!b || !b->active) continue;
        if (b->damagesPlayer) continue;

        for (int i = 0; i < poolSize; i++)
        {
            if (!pool[i].active) continue;

            if (b->lastHitEnemyIndex == i)
            {
                if (!CheckCollisionRecs(pool[i].GetBounds(), b->GetBounds()))
                    b->lastHitEnemyIndex = -1;
                continue;
            }

            if (!CheckCollisionRecs(pool[i].GetBounds(), b->GetBounds())) continue;

            if (b->isBossBigProjectile)
            {
                pool[i].TakeDamage(b->damage);
                b->shouldExplode = true;
                break;
            }

            float ex = pool[i].position.x, ey = pool[i].position.y;
            Color ec = pool[i].color;
            float bvx = b->velocity.x, bvy = b->velocity.y;
            float blen = std::sqrt(bvx * bvx + bvy * bvy);
            if (pool[i].type == EnemyType::ARMORED && pool[i].hasArmor)
            {
                if (b->isShotgunPellet)
                {
                    pool[i].hasArmor = false;
                    if (particlePool) particlePool->SpawnShockwaveEffect(ex, ey, 120.0f);
                    b->Deactivate();
                    break;
                }
                b->lastHitEnemyIndex = i;
                b->hitsLeft--;
                if (b->hitsLeft <= 0) { b->Deactivate(); break; }
                if (b->isRicochet)
                {
                    float bestDistSq = 999999.0f, bestDistSq2 = 999999.0f;
                    int bestK = -1, bestK2 = -1;
                    for (int k = 0; k < poolSize; k++)
                    {
                        if (k == i || !pool[k].active) continue;
                        float dxx = pool[k].position.x - b->position.x;
                        float dyy = pool[k].position.y - b->position.y;
                        float dsq = dxx * dxx + dyy * dyy;
                        if (dsq < 1.0f) continue;
                        if (dsq < bestDistSq) { bestDistSq2 = bestDistSq; bestK2 = bestK; bestDistSq = dsq; bestK = k; }
                        else if (dsq < bestDistSq2) { bestDistSq2 = dsq; bestK2 = k; }
                    }
                    if (bestK >= 0)
                    {
                        float dx = pool[bestK].position.x - b->position.x;
                        float dy = pool[bestK].position.y - b->position.y;
                        float len = std::sqrt(dx * dx + dy * dy);
                        if (len > 0.001f) { b->velocity.x = (dx / len) * b->speed; b->velocity.y = (dy / len) * b->speed; }
                        if (b->ricochetPlusPlus && bestK2 >= 0)
                        {
                            float dx2 = pool[bestK2].position.x - b->position.x;
                            float dy2 = pool[bestK2].position.y - b->position.y;
                            float len2 = std::sqrt(dx2 * dx2 + dy2 * dy2);
                            if (len2 > 0.001f)
                            {
                                float vx2 = (dx2 / len2) * b->speed, vy2 = (dy2 / len2) * b->speed;
                                bulletPool.Spawn(b->position.x, b->position.y, vx2, vy2, b->damage, 1, false, b->radius, false, 1.0f);
                            }
                        }
                    }
                    else b->Deactivate();
                    b->lastHitEnemyIndex = -1;
                }
                else if (blen > 0.001f)
                {
                    float nx = bvx / blen, ny = bvy / blen;
                    float push = pool[i].radius + b->radius + 20.0f;
                    b->position.x += nx * push;
                    b->position.y += ny * push;
                }
                break;
            }
            float knockbackStrength = 180.0f * (b->knockbackMultiplier > 0.0f ? b->knockbackMultiplier : 1.0f) * GameManager::Instance().GetBulletSizeMultiplier();
            if (blen > 1.0f)
                pool[i].ApplyKnockback(bvx, bvy, knockbackStrength);
            pool[i].TakeDamage(b->damage);
            if (!pool[i].active)
            {
                int xpAmount = GameManager::Instance().GetXpPerKill() * pool[i].xpMultiplier;
                if (xpOrbPool) xpOrbPool->SpawnMany(ex, ey, xpAmount);
                if (healthOrbPool && b->isShotgunPellet)
                {
                    int healthOrbCount = GetRandomValue(5, 10);
                    for (int h = 0; h < healthOrbCount; h++)
                        healthOrbPool->Spawn(ex, ey);
                }
                if (particlePool)
                {
                    float dx = (blen > 0.001f) ? (bvx / blen) : 0.0f;
                    float dy = (blen > 0.001f) ? (bvy / blen) : -1.0f;
                    particlePool->SpawnBloodSplatter(ex, ey, ec, 36, dx, dy);
                }
                if (GameManager::Instance().HasDeathExplosionUpgrade() && GetRandomValue(1, 1000) <= (int)(GameManager::Instance().GetExplosionChancePercent() * 10))
                {
                    float explosionRadius = 100.0f * GameManager::Instance().GetExplosionRadiusMultiplier();
                    if (particlePool)
                    {
                        particlePool->SpawnExplosionEffect(ex, ey, explosionRadius, 32);
                        particlePool->SpawnShockwaveEffect(ex, ey, explosionRadius * 1.2f, 48);
                    }
                    const int explosionProjectileCount = 24;
                    const float explosionProjectileSpeed = 480.0f;
                    const int explosionProjectileDamage = 15;
                    for (int p = 0; p < explosionProjectileCount; p++)
                    {
                        float angle = (float)p * (2.0f * 3.14159265f / (float)explosionProjectileCount);
                        float vx = std::cos(angle) * explosionProjectileSpeed;
                        float vy = std::sin(angle) * explosionProjectileSpeed;
                        bulletPool.Spawn(ex, ey, vx, vy, explosionProjectileDamage, 1, false, 5.0f, false, 1.5f);
                    }
                    const int explosionDamage = 15;
                    const float explosionKnockback = 250.0f;
                    for (int k = 0; k < poolSize; k++)
                    {
                        if (!pool[k].active || k == i) continue;
                        float dx = pool[k].position.x - ex;
                        float dy = pool[k].position.y - ey;
                        float dist = std::sqrt(dx * dx + dy * dy);
                        if (dist < explosionRadius && dist > 0.001f)
                        {
                            if (pool[k].type == EnemyType::ARMORED && pool[k].hasArmor) continue;
                            float ndx = dx / dist, ndy = dy / dist;
                            pool[k].ApplyKnockback(ndx, ndy, explosionKnockback);
                            float kx = pool[k].position.x, ky = pool[k].position.y;
                            Color kc = pool[k].color;
                            pool[k].TakeDamage(explosionDamage);
                            if (!pool[k].active)
                            {
                                int xpAmount = GameManager::Instance().GetXpPerKill() * pool[k].xpMultiplier;
                                if (xpOrbPool) xpOrbPool->SpawnMany(kx, ky, xpAmount);
                                if (particlePool)
                                    particlePool->SpawnBloodSplatter(kx, ky, kc, 36, ndx, ndy);
                                if (pool[k].type == EnemyType::SPLITTER)
                                {
                                    const float splitOffset = 28.0f;
                                    Spawn(EnemyType::SPLITTER_SMALL, kx - splitOffset, ky);
                                    Spawn(EnemyType::SPLITTER_SMALL, kx + splitOffset, ky);
                                    Spawn(EnemyType::SPLITTER_SMALL, kx, ky - splitOffset);
                                    Spawn(EnemyType::SPLITTER_SMALL, kx, ky + splitOffset);
                                }
                            }
                        }
                    }
                }
                if (pool[i].type == EnemyType::SPLITTER)
                {
                    const float splitOffset = 28.0f;
                    Spawn(EnemyType::SPLITTER_SMALL, ex - splitOffset, ey);
                    Spawn(EnemyType::SPLITTER_SMALL, ex + splitOffset, ey);
                    Spawn(EnemyType::SPLITTER_SMALL, ex, ey - splitOffset);
                    Spawn(EnemyType::SPLITTER_SMALL, ex, ey + splitOffset);
                }
            }

            b->lastHitEnemyIndex = i;
            b->hitsLeft--;

            if (b->hitsLeft <= 0)
            {
                b->Deactivate();
                break;
            }

            if (b->isRicochet)
            {
                float bestDistSq = 999999.0f, bestDistSq2 = 999999.0f;
                int bestK = -1, bestK2 = -1;
                for (int k = 0; k < poolSize; k++)
                {
                    if (k == i || !pool[k].active) continue;
                    float dxx = pool[k].position.x - b->position.x;
                    float dyy = pool[k].position.y - b->position.y;
                    float dsq = dxx * dxx + dyy * dyy;
                    if (dsq < 1.0f) continue;
                    if (dsq < bestDistSq) { bestDistSq2 = bestDistSq; bestK2 = bestK; bestDistSq = dsq; bestK = k; }
                    else if (dsq < bestDistSq2) { bestDistSq2 = dsq; bestK2 = k; }
                }
                if (bestK >= 0)
                {
                    float dx = pool[bestK].position.x - b->position.x;
                    float dy = pool[bestK].position.y - b->position.y;
                    float len = std::sqrt(dx * dx + dy * dy);
                    if (len > 0.001f)
                    {
                        b->velocity.x = (dx / len) * b->speed;
                        b->velocity.y = (dy / len) * b->speed;
                    }
                    if (b->ricochetPlusPlus && bestK2 >= 0)
                    {
                        float dx2 = pool[bestK2].position.x - b->position.x;
                        float dy2 = pool[bestK2].position.y - b->position.y;
                        float len2 = std::sqrt(dx2 * dx2 + dy2 * dy2);
                        if (len2 > 0.001f)
                        {
                            float vx2 = (dx2 / len2) * b->speed;
                            float vy2 = (dy2 / len2) * b->speed;
                            bulletPool.Spawn(b->position.x, b->position.y, vx2, vy2, b->damage, 1, false, b->radius, false, 1.0f);
                        }
                    }
                }
                else
                    b->Deactivate();
                b->lastHitEnemyIndex = -1;
                break;
            }

            if (blen > 0.001f)
            {
                float nx = bvx / blen, ny = bvy / blen;
                float push = pool[i].radius + b->radius + 20.0f;
                b->position.x += nx * push;
                b->position.y += ny * push;
            }
            break;
        }
    }
}
