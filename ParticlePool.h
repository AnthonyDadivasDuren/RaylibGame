#pragma once

#include "raylib.h"
#include <vector>
#include <cmath>

struct Particle
{
    Vector2 position;
    Vector2 velocity;
    bool active;
    float radius;
    float lifetime;
    float lifetimeMax;
    Color color;
    float gravityAccel;

    Particle() : position({ 0,0 }), velocity({ 0,0 }), active(false), radius(3.0f),
        lifetime(0.0f), lifetimeMax(0.5f), color(RED), gravityAccel(0.0f) {}

    void Activate(float x, float y, float vx, float vy, Color c, float lifeSec, float gravity = 0.0f)
    {
        position = { x, y };
        velocity = { vx, vy };
        active = true;
        lifetime = lifeSec;
        lifetimeMax = lifeSec;
        radius = (float)GetRandomValue(2, 5);
        if (radius < 2.0f) radius = 2.0f;
        color = c;
        gravityAccel = gravity;
    }

    void Deactivate() { active = false; }

    void Update(float deltaTime)
    {
        if (!active) return;
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
        if (gravityAccel != 0.0f) velocity.y += gravityAccel * deltaTime;
        lifetime -= deltaTime;
        if (lifetime <= 0.0f) Deactivate();
    }

    void Draw() const
    {
        if (!active) return;
        float alpha = (lifetimeMax > 0.0f) ? (lifetime / lifetimeMax) : 0.0f;
        if (alpha < 0.0f) alpha = 0.0f;
        if (alpha > 1.0f) alpha = 1.0f;
        Color c = ColorAlpha(color, alpha);
        DrawCircleV(position, radius, c);
    }
};

class ParticlePool
{
    std::vector<Particle> pool;
    int poolSize;

public:
    ParticlePool(int size) : poolSize(size)
    {
        pool.resize(size);
    }

    void UpdateAll(float deltaTime)
    {
        for (int i = 0; i < poolSize; i++)
            pool[i].Update(deltaTime);
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

    void SpawnParticleExplosion(float x, float y, Color baseColor, int count = 36)
    {
        for (int i = 0; i < count; i++)
        {
            float angle = (float)GetRandomValue(0, 360) * 3.14159265f / 180.0f;
            float speed = (float)(80 + GetRandomValue(0, 120));
            float vx = cosf(angle) * speed;
            float vy = sinf(angle) * speed;
            float life = 0.45f + (float)GetRandomValue(0, 30) / 100.0f;
            for (int j = 0; j < poolSize; j++)
            {
                if (!pool[j].active)
                {
                    pool[j].Activate(x, y, vx, vy, baseColor, life, 0.0f);
                    break;
                }
            }
        }
    }

    void SpawnBloodSplatter(float x, float y, Color baseColor, int count, float dirX, float dirY)
    {
        (void)dirX;
        (void)dirY;
        for (int i = 0; i < count; i++)
        {
            float angle = (float)GetRandomValue(0, 360) * 3.14159265f / 180.0f;
            float speed = (float)(80 + GetRandomValue(0, 120));
            float vx = cosf(angle) * speed;
            float vy = sinf(angle) * speed;
            float life = 0.45f + (float)GetRandomValue(0, 30) / 100.0f;
            for (int j = 0; j < poolSize; j++)
            {
                if (!pool[j].active)
                {
                    pool[j].Activate(x, y, vx, vy, baseColor, life, 0.0f);
                    break;
                }
            }
        }
    }

    void SpawnExplosionEffect(float x, float y, float radius, int count = 28)
    {
        float baseSpeed = radius / 0.35f;
        Color colors[] = { ORANGE, GOLD, MAROON };
        for (int i = 0; i < count; i++)
        {
            float angle = (float)GetRandomValue(0, 360) * 3.14159265f / 180.0f;
            float speed = baseSpeed * (0.7f + (float)GetRandomValue(0, 30) / 100.0f);
            float vx = cosf(angle) * speed;
            float vy = sinf(angle) * speed;
            float life = 0.3f + (float)GetRandomValue(0, 15) / 100.0f;
            Color c = colors[GetRandomValue(0, 2)];
            for (int j = 0; j < poolSize; j++)
            {
                if (!pool[j].active)
                {
                    pool[j].Activate(x, y, vx, vy, c, life);
                    break;
                }
            }
        }
    }

    void SpawnShockwaveEffect(float x, float y, float radius, int count = 48)
    {
        float baseSpeed = radius / 0.25f;
        Color colors[] = { LIGHTGRAY, GRAY, WHITE };
        for (int i = 0; i < count; i++)
        {
            float angle = (float)GetRandomValue(0, 360) * 3.14159265f / 180.0f;
            float speed = baseSpeed * (0.8f + (float)GetRandomValue(0, 40) / 100.0f);
            float vx = cosf(angle) * speed;
            float vy = sinf(angle) * speed;
            float life = 0.2f + (float)GetRandomValue(0, 15) / 100.0f;
            Color c = colors[GetRandomValue(0, 2)];
            for (int j = 0; j < poolSize; j++)
            {
                if (!pool[j].active)
                {
                    pool[j].Activate(x, y, vx, vy, c, life, 0.0f);
                    break;
                }
            }
        }
    }

    void SpawnMuzzleFlash(float x, float y, float dirX, float dirY, int count = 52)
    {
        float baseAngle = std::atan2(dirY, dirX);
        float coneRad = 0.5f;
        Color colors[] = { GOLD, ORANGE, YELLOW, Color{ 255, 200, 100, 255 } };
        for (int i = 0; i < count; i++)
        {
            float angle = baseAngle + ((float)GetRandomValue(-100, 100) / 100.0f) * coneRad;
            float speed = (float)(180 + GetRandomValue(0, 220));
            float vx = cosf(angle) * speed;
            float vy = sinf(angle) * speed;
            float life = 0.08f + (float)GetRandomValue(0, 15) / 100.0f;
            Color c = colors[GetRandomValue(0, 3)];
            float ox = x + dirX * 18.0f;
            float oy = y + dirY * 18.0f;
            for (int j = 0; j < poolSize; j++)
            {
                if (!pool[j].active)
                {
                    pool[j].Activate(ox, oy, vx, vy, c, life);
                    break;
                }
            }
        }
    }
};
