#pragma once
#include "Core/GameState.h"
#include "Pools/EnemyPool.h"
#include "Pools/BulletPool.h"
#include "Pools/ParticlePool.h"
#include "Pools/XpOrbPool.h"
#include "Pools/AmmoOrbPool.h"
#include "Pools/HealthOrbPool.h"

class StateManager
{
    std::map<std::string, GameState*> states;
    GameState* currentState = nullptr;
    EnemyPool* enemyPool = nullptr;
    BulletPool* bulletPool = nullptr;
    ParticlePool* particlePool = nullptr;
    XpOrbPool* xpOrbPool = nullptr;
    AmmoOrbPool* ammoOrbPool = nullptr;
    HealthOrbPool* healthOrbPool = nullptr;

public:
    void SetPools(EnemyPool* ep, BulletPool* bp, ParticlePool* pp = nullptr, XpOrbPool* xop = nullptr, AmmoOrbPool* aop = nullptr, HealthOrbPool* hop = nullptr)
    {
        enemyPool = ep;
        bulletPool = bp;
        particlePool = pp;
        xpOrbPool = xop;
        ammoOrbPool = aop;
        healthOrbPool = hop;
    }

    ~StateManager()
    {
        for (auto& p : states)
            delete p.second;
        states.clear();
        currentState = nullptr;
    }

    void RegisterState(const std::string& name, GameState* state)
    {
        states[name] = state;
    }

    void ChangeState(const std::string& name)
    {
        auto it = states.find(name);
        if (it == states.end()) return;
        if (currentState)
            currentState->Exit();
        currentState = it->second;
        currentState->Enter();
        if (name == "Playing" && enemyPool) enemyPool->ResetPool();
        if (name == "Playing" && bulletPool) bulletPool->ResetPool();
        if (name == "Playing" && particlePool) particlePool->ResetPool();
        if (name == "Playing" && xpOrbPool) xpOrbPool->ResetPool();
        if (name == "Playing" && ammoOrbPool) ammoOrbPool->ResetPool();
        if (name == "Playing" && healthOrbPool) healthOrbPool->ResetPool();
    }

    void Update(float deltaTime, EnemyPool* enemyPool, BulletPool* bulletPool, ParticlePool* particlePool, XpOrbPool* xpOrbPool, AmmoOrbPool* ammoOrbPool, HealthOrbPool* healthOrbPool)
    {
        if (!currentState) return;
        std::string next = currentState->Update(deltaTime, enemyPool, bulletPool, particlePool, xpOrbPool, ammoOrbPool, healthOrbPool);
        if (!next.empty())
            ChangeState(next);
    }

    void Draw(EnemyPool* enemyPool, BulletPool* bulletPool, ParticlePool* particlePool, XpOrbPool* xpOrbPool, AmmoOrbPool* ammoOrbPool, HealthOrbPool* healthOrbPool)
    {
        if (currentState)
            currentState->Draw(enemyPool, bulletPool, particlePool, xpOrbPool, ammoOrbPool, healthOrbPool);
    }
};
