#pragma once

#include <vector>
#include <set>

struct DashTrailPoint { float x; float y; };

class EnemyPool;
class ParticlePool;
class XpOrbPool;
class AmmoOrbPool;
class GameManager;
class Player;
class ShockwaveSystem;
#include "Shared/ExpandingShockwave.h"

class DashSystem
{
public:
    static const int TrailMax = 14;

    DashSystem();

    bool IsDashing() const { return timer_ > 0.0f; }
    float GetTimer() const { return timer_; }
    float GetDirectionX() const { return dirX_; }
    float GetDirectionY() const { return dirY_; }

    void StartDash(float dirX, float dirY, float duration);

    void Tick(float deltaTime, float dashLength, float dashDuration, float& outDx, float& outDy);

    bool HasHitEnemy(int enemyIndex) const { return hitIndices_.count(enemyIndex) != 0; }
    void RecordHitEnemy(int enemyIndex) { hitIndices_.insert(enemyIndex); }
    void ClearHitEnemies() { hitIndices_.clear(); }

    void AddTrailPoint(float x, float y);
    const std::vector<DashTrailPoint>& GetTrail() const { return trail_; }
    void ClearTrail() { trail_.clear(); }

    void DrawTrail(float playerSize) const;

    void Reset();

    void UpdateRechargeAndInput(float deltaTime, GameManager* gm, Player* player, ParticlePool* particlePool);

    void ApplyDashDamage(float playerX, float playerY, EnemyPool* enemyPool, ParticlePool* particlePool,
                         XpOrbPool* xpOrbPool, AmmoOrbPool* ammoOrbPool, GameManager* gm,
                         ShockwaveSystem* shockwaveSystem);

private:
    float timer_;
    float dirX_, dirY_;
    float rechargeTimer_;
    std::set<int> hitIndices_;
    std::vector<DashTrailPoint> trail_;
};
