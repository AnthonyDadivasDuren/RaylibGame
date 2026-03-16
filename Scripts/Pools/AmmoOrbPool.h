#pragma once

#include "raylib.h"
#include "Core/GameManager.h"
#include <vector>
#include <cmath>

struct AmmoOrb
{
    Vector2 position;
    Vector2 velocity;
    bool active;
    float radius;
    float lifeTime;

    AmmoOrb() : position({ 0, 0 }), velocity({ 0, 0 }), active(false), radius(5.0f), lifeTime(0.0f) {}

    void Activate(float x, float y)
    {
        position.x = x;
        position.y = y;
        velocity.x = 0;
        velocity.y = 0;
        active = true;
        radius = 5.0f;
        lifeTime = 0.0f;
    }

    void Deactivate() { active = false; }

    void Update(float deltaTime, float playerX, float playerY)
    {
        if (!active) return;
        lifeTime += deltaTime;
        if (lifeTime >= 10.0f)
        {
            Deactivate();
            return;
        }
        float dx = playerX - position.x;
        float dy = playerY - position.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        const float baseMagnetRadius = 200.0f;
        float magnetRadius = baseMagnetRadius * GameManager::Instance().GetOrbMagnetismMultiplier();
        const float collectRadius = 45.0f;
        const float magnetSpeed = 280.0f;
        if (dist < collectRadius)
        {
            GameManager::Instance().AddShotgunAmmo(1);
            Deactivate();
            return;
        }
        if (dist < magnetRadius && dist > 0.001f)
        {
            float nx = dx / dist;
            float ny = dy / dist;
            velocity.x = nx * magnetSpeed;
            velocity.y = ny * magnetSpeed;
        }
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
        velocity.x *= 0.92f;
        velocity.y *= 0.92f;
    }

    void Draw() const
    {
        if (!active) return;
        DrawCircleV(position, radius, GOLD);
        DrawCircleLines((int)position.x, (int)position.y, radius, ORANGE);
    }
};

class AmmoOrbPool
{
    std::vector<AmmoOrb> pool;
    int poolSize;

public:
    AmmoOrbPool(int size) : poolSize(size)
    {
        pool.resize(size);
    }

    void Spawn(float x, float y)
    {
        float offsetX = (float)(GetRandomValue(-8, 8));
        float offsetY = (float)(GetRandomValue(-8, 8));
        for (int i = 0; i < poolSize; i++)
        {
            if (!pool[i].active)
            {
                pool[i].Activate(x + offsetX, y + offsetY);
                return;
            }
        }
    }

    void UpdateAll(float deltaTime, float playerX, float playerY)
    {
        for (int i = 0; i < poolSize; i++)
            pool[i].Update(deltaTime, playerX, playerY);
    }

    void DrawAll() const
    {
        for (int i = 0; i < poolSize; i++)
            pool[i].Draw();
    }

    void ResetPool()
    {
        for (int i = 0; i < poolSize; i++)
            pool[i].Deactivate();
    }

    int GetPoolSize() const { return poolSize; }
};
