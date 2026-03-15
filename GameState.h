#pragma once

#include "raylib.h"
#include "GameManager.h"
#include <string>
#include <map>

class BulletPool;
class EnemyPool;
class ParticlePool;
class XpOrbPool;
class AmmoOrbPool;
class HealthOrbPool;

class GameState
{
public:
    virtual ~GameState() {}

    virtual void Enter() {}
    virtual void Exit() {}

    virtual std::string Update(float deltaTime, EnemyPool* enemyPool, BulletPool* bulletPool, ParticlePool* particlePool, XpOrbPool* xpOrbPool, AmmoOrbPool* ammoOrbPool, HealthOrbPool* healthOrbPool) = 0;
    virtual void Draw(EnemyPool* enemyPool, BulletPool* bulletPool, ParticlePool* particlePool, XpOrbPool* xpOrbPool, AmmoOrbPool* ammoOrbPool, HealthOrbPool* healthOrbPool) = 0;

    virtual const char* GetName() const = 0;
};