#pragma once

#include "raylib.h"
#include <vector>
#include <cmath>

struct Bullet
{
    static const int trailSize = 16;
    Vector2 position;
    Vector2 velocity;
    bool active;
    float radius;
    int damage;
    int hitsLeft;
    bool isRicochet;
    bool ricochetPlusPlus;
    float speed;
    int lastHitEnemyIndex;
    float knockbackMultiplier;
    Vector2 trail[trailSize];
    int trailCount;
    bool isShotgunPellet;
    bool damagesPlayer;
    Color drawColor;
    bool isBossBigProjectile;
    float bossBigLifetime;
    bool isBossHomingExplosion;
    float bossHomingTimeLeft;
    bool shouldExplode;

    Bullet() : position({ 0,0 }), velocity({ 0,0 }), active(false), radius(10.0f), damage(10),
        hitsLeft(1), isRicochet(false), ricochetPlusPlus(false), speed(400.0f), lastHitEnemyIndex(-1), knockbackMultiplier(1.0f), trailCount(0), isShotgunPellet(false), damagesPlayer(false),
        drawColor(YELLOW), isBossBigProjectile(false), bossBigLifetime(0.0f), isBossHomingExplosion(false), bossHomingTimeLeft(0.0f), shouldExplode(false)
    {
        for (int i = 0; i < trailSize; i++) trail[i] = { 0.0f, 0.0f };
    }

    void Activate(float x, float y, float vx, float vy, int dmg, int hits = 1, bool ricochet = false, float r = 10.0f, bool ricochetPP = false, float knockbackMult = 1.0f, bool shotgunPellet = false, bool damagePlayer = false, Color color = YELLOW, bool bossBig = false, float bossBigLifetimeSec = 0.0f, bool bossHoming = false, float bossHomingSec = 0.0f)
    {
        position = { x, y };
        velocity = { vx, vy };
        damage = dmg;
        radius = r;
        hitsLeft = hits;
        isRicochet = ricochet;
        ricochetPlusPlus = ricochetPP;
        knockbackMultiplier = knockbackMult;
        lastHitEnemyIndex = -1;
        trailCount = 0;
        isShotgunPellet = shotgunPellet;
        damagesPlayer = damagePlayer;
        drawColor = color;
        isBossBigProjectile = bossBig;
        bossBigLifetime = bossBigLifetimeSec;
        isBossHomingExplosion = bossHoming;
        bossHomingTimeLeft = bossHomingSec;
        shouldExplode = false;
        float len = std::sqrt(vx * vx + vy * vy);
        speed = (len > 0.001f) ? len : 400.0f;
        active = true;
    }

    void Deactivate() { active = false; lastHitEnemyIndex = -1; trailCount = 0; isBossBigProjectile = false; isBossHomingExplosion = false; shouldExplode = false; }

    void SetDrawColor(Color c) { drawColor = c; }

    void Update(float deltaTime, int screenWidth, int screenHeight, float playerX = -1.0f, float playerY = -1.0f)
    {
        if (!active) return;
        if (isBossBigProjectile && bossBigLifetime > 0.0f)
        {
            bossBigLifetime -= deltaTime;
            if (bossBigLifetime <= 0.0f) shouldExplode = true;
        }
        if (isBossHomingExplosion && bossHomingTimeLeft > 0.0f && playerX >= 0.0f && playerY >= 0.0f)
        {
            float dx = playerX - position.x;
            float dy = playerY - position.y;
            float len = std::sqrt(dx * dx + dy * dy);
            if (len > 0.001f)
            {
                float toPlayerX = dx / len;
                float toPlayerY = dy / len;
                const float steerStrength = 2.0f * deltaTime;
                velocity.x += (toPlayerX * speed - velocity.x) * steerStrength;
                velocity.y += (toPlayerY * speed - velocity.y) * steerStrength;
                float vlen = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
                if (vlen > 0.001f) { velocity.x = (velocity.x / vlen) * speed; velocity.y = (velocity.y / vlen) * speed; }
            }
            bossHomingTimeLeft -= deltaTime;
            if (bossHomingTimeLeft <= 0.0f) isBossHomingExplosion = false;
        }
        Vector2 prev = position;
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
        if (trailCount < trailSize)
        {
            trail[trailCount] = prev;
            trailCount++;
        }
        else
        {
            for (int i = trailSize - 1; i >= 1; i--)
                trail[i] = trail[i - 1];
            trail[0] = prev;
        }
        if (position.x < -20 || position.x > screenWidth + 20 ||
            position.y < -20 || position.y > screenHeight + 20)
        {
            if (isBossBigProjectile) shouldExplode = true;
            else Deactivate();
        }
    }

    void Draw() const
    {
        if (!active) return;
        float trailRadius = radius * 0.6f;
        if (trailRadius < 2.0f) trailRadius = 2.0f;
        for (int i = 0; i < trailCount; i++)
        {
            float alpha = 0.12f + 0.38f * (1.0f - (float)i / (float)(trailCount > 0 ? trailCount : 1));
            if (alpha > 1.0f) alpha = 1.0f;
            DrawCircleV(trail[i], trailRadius, ColorAlpha(drawColor, alpha));
        }
        if (trailCount > 0)
        {
            DrawLineEx(trail[0], position, trailRadius * 1.8f, ColorAlpha(drawColor, 0.35f));
        }
        DrawCircleV(position, radius, drawColor);
    }

    Rectangle GetBounds() const
    {
        return { position.x - radius, position.y - radius, radius * 2, radius * 2 };
    }
};

class BulletPool
{
    std::vector<Bullet> pool;
    int poolSize;
    int screenWidth;
    int screenHeight;

public:
    BulletPool(int size, int w = 800, int h = 600) : poolSize(size), screenWidth(w), screenHeight(h)
    {
        pool.resize(size);
    }

    Bullet* Spawn(float x, float y, float vx, float vy, int damage, int hitsLeft = 1, bool isRicochet = false, float radius = 10.0f, bool ricochetPlusPlus = false, float knockbackMultiplier = 1.0f, bool isShotgunPellet = false, bool damagesPlayer = false, Color drawColor = YELLOW, bool isBossBig = false, float bossBigLifetimeSec = 0.0f, bool isBossHoming = false, float bossHomingSec = 0.0f)
    {
        for (int i = 0; i < poolSize; i++)
        {
            if (!pool[i].active)
            {
                pool[i].Activate(x, y, vx, vy, damage, hitsLeft, isRicochet, radius, ricochetPlusPlus, knockbackMultiplier, isShotgunPellet, damagesPlayer, drawColor, isBossBig, bossBigLifetimeSec, isBossHoming, bossHomingSec);
                return &pool[i];
            }
        }
        return nullptr;
    }

    void UpdateAll(float deltaTime, float playerX = -1.0f, float playerY = -1.0f)
    {
        for (int i = 0; i < poolSize; i++)
            pool[i].Update(deltaTime, screenWidth, screenHeight, playerX, playerY);
    }

    void HandleBossBigExplosions(float playerX, float playerY)
    {
        for (int j = 0; j < poolSize; j++)
        {
            Bullet* b = &pool[j];
            if (!b->active) continue;
            if (b->isBossBigProjectile)
            {
                float pdx = b->position.x - playerX, pdy = b->position.y - playerY;
                float pdist = std::sqrt(pdx * pdx + pdy * pdy);
                if (pdist < 100.0f) b->shouldExplode = true;
            }
            if (!b->shouldExplode) continue;
            float ex = b->position.x, ey = b->position.y;
            const int smallCount = 24;
            const float smallSpeed = 480.0f;
            const int smallDamage = 6;
            for (int i = 0; i < smallCount; i++)
            {
                float angle = (float)i * (360.0f / (float)smallCount) * 3.14159265f / 180.0f;
                float vx = std::cos(angle) * smallSpeed;
                float vy = std::sin(angle) * smallSpeed;
                Spawn(ex, ey, vx, vy, smallDamage, 1, false, 7.5f, false, 1.0f, false, true, RED);
            }
            b->Deactivate();
        }
    }

    void DrawAll() const
    {
        for (int i = 0; i < poolSize; i++)
            pool[i].Draw();
    }

    Bullet* GetBullet(int index)
    {
        if (index >= 0 && index < poolSize) return &pool[index];
        return nullptr;
    }

    int GetPoolSize() const { return poolSize; }

    void ResetPool()
    {
        for (int i = 0; i < poolSize; i++)
            pool[i].Deactivate();
    }
};
