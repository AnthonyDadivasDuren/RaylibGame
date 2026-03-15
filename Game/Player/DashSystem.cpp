#include "DashSystem.h"
#include "Player.h"
#include "../Enemies/ShockwaveSystem.h"
#include "../../EnemyPool.h"
#include "../../ParticlePool.h"
#include "../../XpOrbPool.h"
#include "../../AmmoOrbPool.h"
#include "../../GameManager.h"
#include "../../ExpandingShockwave.h"
#include "raylib.h"
#include <cmath>
#include <algorithm>

DashSystem::DashSystem() : timer_(0.0f), dirX_(1.0f), dirY_(0.0f), rechargeTimer_(0.0f)
{
}

void DashSystem::StartDash(float dirX, float dirY, float duration)
{
    dirX_ = dirX;
    dirY_ = dirY;
    float dlen = std::sqrt(dirX_ * dirX_ + dirY_ * dirY_);
    if (dlen < 0.001f) { dirX_ = 1.0f; dirY_ = 0.0f; }
    else { dirX_ /= dlen; dirY_ /= dlen; }
    timer_ = duration;
    hitIndices_.clear();
    trail_.clear();
}

void DashSystem::Tick(float deltaTime, float dashLength, float dashDuration, float& outDx, float& outDy)
{
    outDx = 0.0f;
    outDy = 0.0f;
    if (timer_ <= 0.0f) return;
    float dashSpeed = dashLength / dashDuration;
    outDx = dirX_ * dashSpeed * deltaTime;
    outDy = dirY_ * dashSpeed * deltaTime;
    timer_ -= deltaTime;
    if (timer_ < 0.0f) timer_ = 0.0f;
}

void DashSystem::AddTrailPoint(float x, float y)
{
    trail_.push_back({ x, y });
    if ((int)trail_.size() > TrailMax)
        trail_.erase(trail_.begin());
}

void DashSystem::DrawTrail(float playerSize) const
{
    const std::vector<DashTrailPoint>& trail = GetTrail();
    for (size_t t = 0; t < trail.size(); t++)
    {
        float alpha = 0.15f + 0.35f * (float)(t + 1) / (float)(trail.size() + 1);
        DrawRectangle((int)(trail[t].x - playerSize * 0.5f), (int)(trail[t].y - playerSize * 0.5f), (int)playerSize, (int)playerSize, ColorAlpha(BLUE, alpha));
    }
}

void DashSystem::Reset()
{
    timer_ = 0.0f;
    dirX_ = 1.0f;
    dirY_ = 0.0f;
    rechargeTimer_ = 0.0f;
    hitIndices_.clear();
    trail_.clear();
}

void DashSystem::UpdateRechargeAndInput(float deltaTime, GameManager* gm, Player* player, ParticlePool* particlePool)
{
    if (!gm || !player) return;
    if (gm->GetDashCharges() < gm->GetMaxDashCharges())
    {
        rechargeTimer_ -= deltaTime;
        if (rechargeTimer_ <= 0.0f)
        {
            gm->AddDashCharge();
            rechargeTimer_ = gm->GetDashCooldown();
        }
    }
    if (gm->HasDashUpgrade() && IsKeyPressed(KEY_SPACE) && gm->ConsumeDashCharge())
    {
        StartDash(player->GetLastMoveDirX(), player->GetLastMoveDirY(), gm->GetDashDuration());
        rechargeTimer_ = gm->GetDashCooldown();
        player->SetInvincibleTimer(gm->GetDashDuration());
        if (particlePool)
            particlePool->SpawnShockwaveEffect(player->GetCenterX(), player->GetCenterY(), 55);
    }
}

void DashSystem::ApplyDashDamage(float playerX, float playerY, EnemyPool* enemyPool, ParticlePool* particlePool,
                                 XpOrbPool* xpOrbPool, AmmoOrbPool* ammoOrbPool, GameManager* gm,
                                 ShockwaveSystem* shockwaveSystem)
{
    if (!enemyPool || !ammoOrbPool || !shockwaveSystem) return;

    const float dashShockwaveRadius = 70.0f;
    for (int i = 0; i < enemyPool->GetPoolSize(); i++)
    {
        if (HasHitEnemy(i)) continue;
        Enemy* e = enemyPool->GetEnemy(i);
        if (!e || !e->active) continue;
        float dist = std::sqrt((playerX - e->position.x) * (playerX - e->position.x) + (playerY - e->position.y) * (playerY - e->position.y));
        if (dist >= dashShockwaveRadius) continue;
        RecordHitEnemy(i);
        float ex = e->position.x, ey = e->position.y;
        Color ec = e->color;
        int xpMult = e->xpMultiplier;
        EnemyType etype = e->type;
        if (e->type == EnemyType::ARMORED && e->hasArmor)
        {
            e->hasArmor = false;
            ExpandingShockwave sw;
            sw.x = ex; sw.y = ey;
            sw.currentRadius = 5.0f;
            sw.maxRadius = 120.0f;
            sw.expansionSpeed = 380.0f;
            sw.excludeEnemyIndex = i;
            shockwaveSystem->AddShockwave(sw);
            if (particlePool) particlePool->SpawnShockwaveEffect(ex, ey, 120.0f);
        }
        else
        {
            e->TakeDamage(100);
            if (!e->active)
            {
                if (xpOrbPool && gm) xpOrbPool->SpawnMany(ex, ey, gm->GetXpPerKill() * xpMult);
                if (particlePool) particlePool->SpawnBloodSplatter(ex, ey, ec, 36, dirX_, dirY_);
                ammoOrbPool->Spawn(ex, ey);
                if (etype == EnemyType::SPLITTER)
                {
                    const float splitOffset = 28.0f;
                    enemyPool->Spawn(EnemyType::SPLITTER_SMALL, ex - splitOffset, ey);
                    enemyPool->Spawn(EnemyType::SPLITTER_SMALL, ex + splitOffset, ey);
                    enemyPool->Spawn(EnemyType::SPLITTER_SMALL, ex, ey - splitOffset);
                    enemyPool->Spawn(EnemyType::SPLITTER_SMALL, ex, ey + splitOffset);
                }
            }
        }
    }
}
