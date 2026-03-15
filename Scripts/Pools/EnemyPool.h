#pragma once

#include "raylib.h"
#include "Shared/EnemyFactory.h"
#include <vector>
#include <cmath>
#include "Pools/BulletPool.h"
#include "Pools/ParticlePool.h"
#include "Pools/XpOrbPool.h"
#include "Pools/HealthOrbPool.h"
#include "Core/GameManager.h"

struct Enemy
{
    Vector2 position;
    Vector2 velocity;
    int health;
    int maxHealth;
    EnemyType type;
    float radius;
    Color color;
    bool active;
    float hitFlashTimer;
    Vector2 knockbackVelocity;
    bool isSpecial;
    int xpMultiplier;
    bool hasArmor;

    Enemy() : position({ 0,0 }), velocity({ 0,0 }), health(0), maxHealth(0),
        type(EnemyType::GRUNT), radius(20.0f), color(RED), active(false), hitFlashTimer(0.0f), knockbackVelocity({ 0,0 }),
        isSpecial(false), xpMultiplier(1), hasArmor(false) {
    }

    void Activate(float x, float y, EnemyType enemyType);
    void Deactivate();
    void ApplyKnockback(float vx, float vy, float strength);
    void Update(float deltaTime);
    void TakeDamage(int amount);
    void Draw() const;
    Rectangle GetBounds() const;
};

class EnemyPool
{
    std::vector<Enemy> pool;
    int poolSize;

public:
    EnemyPool(int size);
    Enemy* Spawn(EnemyType type, float x, float y);

    void UpdateAll(float deltaTime);
    void ResolveEnemyOverlaps();
    void DrawAll() const;
    Enemy* GetEnemy(int index);
    int GetPoolSize() const;
    int GetActiveCount() const;
    void ResetPool();

    void ShockwaveAt(float x, float y, float radius, int damage, int excludeIndex, ParticlePool* particlePool = nullptr, XpOrbPool* xpOrbPool = nullptr);

    void CheckCollisions(BulletPool& bulletPool, ParticlePool* particlePool = nullptr, XpOrbPool* xpOrbPool = nullptr, HealthOrbPool* healthOrbPool = nullptr);
};
