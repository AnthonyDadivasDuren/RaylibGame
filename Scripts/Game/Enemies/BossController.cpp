#include "Game/Enemies/BossController.h"
#include "Core/GameManager.h"
#include "Game/Player/Player.h"
#include "Pools/BulletPool.h"
#include "Pools/ParticlePool.h"
#include "Pools/EnemyPool.h"
#include "Pools/XpOrbPool.h"
#include "Shared/EnemyFactory.h"
#include "raylib.h"
#include <cmath>

BossController::BossController()
    : active_(false)
    , x_(0.0f), y_(0.0f)
    , health_(0), maxHealth_(7000)
    , radius_(108.0f)
    , state_(0), stateTimer_(0.0f)
    , dashTimer_(0.0f)
    , tellDirX_(0.0f), tellDirY_(0.0f)
    , dashDirX_(1.0f), dashDirY_(0.0f)
    , dashTargetX_(0.0f), dashTargetY_(0.0f)
    , tellDuration_(0.4f), dashDuration_(0.18f)
    , dashSpeed_(2800.0f), tellBackDist_(90.0f)
    , explosionSpeed_(960.0f)
    , explosionFlashTimer_(0.0f)
    , hitFlashTimer_(0.0f)
    , idleMoveSpeed_(76.0f)
    , dashDamageToPlayer_(8.4f)
    , bossSpawnTriggered_(false)
    , bossSpawnDelayTimer_(0.0f)
    , wave10FlashTimer_(0.0f)
{
}

void BossController::ResetWave10State()
{
    bossSpawnTriggered_ = false;
    bossSpawnDelayTimer_ = 0.0f;
    wave10FlashTimer_ = 0.0f;
}

static void ClearEnemiesAndSpawnXp(EnemyPool* enemyPool, XpOrbPool* xpOrbPool, GameManager* gm)
{
    if (!enemyPool || !xpOrbPool || !gm) return;
    for (int i = 0; i < enemyPool->GetPoolSize(); i++)
    {
        Enemy* e = enemyPool->GetEnemy(i);
        if (!e || !e->active) continue;
        int xpAmount = gm->GetXpPerKill() * e->xpMultiplier;
        xpOrbPool->SpawnMany(e->position.x, e->position.y, xpAmount);
    }
    enemyPool->ResetPool();
}

void BossController::OnWave10WaveComplete(EnemyPool* enemyPool, XpOrbPool* xpOrbPool, GameManager* gm)
{
    ClearEnemiesAndSpawnXp(enemyPool, xpOrbPool, gm);
    bossSpawnTriggered_ = true;
    bossSpawnDelayTimer_ = 5.0f;
}

void BossController::UpdateWave10Transition(float deltaTime, EnemyPool* enemyPool, XpOrbPool* xpOrbPool, GameManager* gm, float spawnCenterX, float spawnCenterY)
{
    if (active_) return;
    if (!bossSpawnTriggered_) return;

    if (bossSpawnDelayTimer_ > 0.0f)
    {
        bossSpawnDelayTimer_ -= deltaTime;
        if (bossSpawnDelayTimer_ <= 0.0f)
        {
            ClearEnemiesAndSpawnXp(enemyPool, xpOrbPool, gm);
            wave10FlashTimer_ = 1.0f;
        }
        return;
    }

    if (wave10FlashTimer_ > 0.0f)
    {
        wave10FlashTimer_ -= deltaTime;
        if (wave10FlashTimer_ <= 0.0f)
            Spawn(spawnCenterX, spawnCenterY);
    }
}

void BossController::Spawn(float centerX, float centerY)
{
    active_ = true;
    x_ = centerX;
    y_ = centerY;
    health_ = maxHealth_;
    state_ = 0;
    stateTimer_ = 0.0f;
    dashTimer_ = (3.0f + (float)GetRandomValue(0, 200) / 100.0f) * 0.7f;
    hitFlashTimer_ = 0.0f;
}

void BossController::Update(float deltaTime, float playerX, float playerY, float playerSize, bool playerVulnerable,
                            float playerLastMoveX, float playerLastMoveY, float playerSpeedMult,
                            int screenWidth, int screenHeight,
                            GameManager* gm, Player* player, BulletPool* bulletPool, ParticlePool* particlePool, EnemyPool* enemyPool)
{
    if (!active_) return;

    TickExplosionFlashTimer(deltaTime);

    if (state_ == 0)
    {
        float dx = playerX - x_, dy = playerY - y_;
        float len = std::sqrt(dx * dx + dy * dy);
        if (len > 0.001f)
        {
            dx /= len; dy /= len;
            x_ += dx * idleMoveSpeed_ * deltaTime;
            y_ += dy * idleMoveSpeed_ * deltaTime;
        }
        dashTimer_ -= deltaTime;
        if (dashTimer_ <= 0.0f)
        {
            float dx = playerX - x_, dy = playerY - y_;
            float len = std::sqrt(dx * dx + dy * dy);
            if (len > 0.001f) { dx /= len; dy /= len; }
            tellDirX_ = -dx; tellDirY_ = -dy;
            state_ = 1;
            stateTimer_ = tellDuration_;
        }
    }
    else if (state_ == 1)
    {
        x_ += tellDirX_ * (tellBackDist_ / tellDuration_) * deltaTime;
        y_ += tellDirY_ * (tellBackDist_ / tellDuration_) * deltaTime;
        stateTimer_ -= deltaTime;
        if (stateTimer_ <= 0.0f)
        {
            float leadTime = 0.25f;
            float effectiveSpeed = (playerSpeedMult > 0.001f) ? (200.0f * playerSpeedMult) : 200.0f;
            dashTargetX_ = playerX + playerLastMoveX * effectiveSpeed * leadTime;
            dashTargetY_ = playerY + playerLastMoveY * effectiveSpeed * leadTime;
            float dx = dashTargetX_ - x_, dy = dashTargetY_ - y_;
            float dlen = std::sqrt(dx * dx + dy * dy);
            if (dlen > 0.001f) { dashDirX_ = dx / dlen; dashDirY_ = dy / dlen; }
            else { dashDirX_ = 1.0f; dashDirY_ = 0.0f; }
            state_ = 2;
        }
    }
    else if (state_ == 2)
    {
        float toTargetX = dashTargetX_ - x_, toTargetY = dashTargetY_ - y_;
        float distToTarget = std::sqrt(toTargetX * toTargetX + toTargetY * toTargetY);
        float move = dashSpeed_ * deltaTime;
        bool reached = (distToTarget <= 50.0f) || (distToTarget <= move);
        if (!reached)
        {
            x_ += dashDirX_ * move;
            y_ += dashDirY_ * move;
            toTargetX = dashTargetX_ - x_; toTargetY = dashTargetY_ - y_;
            if (toTargetX * dashDirX_ + toTargetY * dashDirY_ < 0.0f) reached = true;
        }
        else if (distToTarget > 0.001f && distToTarget <= move)
        {
            x_ = dashTargetX_;
            y_ = dashTargetY_;
        }

        float distToPlayer = std::sqrt((playerX - x_) * (playerX - x_) + (playerY - y_) * (playerY - y_));
        if (distToPlayer < playerSize * 0.5f + radius_ && playerVulnerable && gm && player)
        {
            gm->TakeDamage((int)dashDamageToPlayer_);
            player->SetHitFlashTimer(0.4f);
            player->SetHitInvincibleTimer(0.4f);
        }

        if (reached)
        {
            explosionFlashTimer_ = 0.25f;
            if (particlePool)
            {
                particlePool->SpawnExplosionEffect(x_, y_, 120.0f, 42);
                particlePool->SpawnShockwaveEffect(x_, y_, 140.0f, 64);
            }
            if (bulletPool)
            {
                for (int i = 0; i < ExplosionProjectileCount; i++)
                {
                    float angle = (float)i * (360.0f / (float)ExplosionProjectileCount) * 3.14159265f / 180.0f;
                    float vx = std::cos(angle) * explosionSpeed_;
                    float vy = std::sin(angle) * explosionSpeed_;
                    bulletPool->Spawn(x_, y_, vx, vy, ExplosionDamage, 1, false, 15.0f, false, 1.0f, false, true, RED, false, 0.0f, true, 0.5f);
                }
                float bdx = playerX - x_, bdy = playerY - y_;
                float blen = std::sqrt(bdx * bdx + bdy * bdy);
                if (blen < 0.001f) { bdx = 1.0f; bdy = 0.0f; }
                else { bdx /= blen; bdy /= blen; }
                const float bigSpeed = 440.0f;
                const float bigRadius = 33.0f;
                const int bigDamage = 17;
                bulletPool->Spawn(x_, y_, bdx * bigSpeed, bdy * bigSpeed, bigDamage, 1, false, bigRadius, false, 1.0f, false, false, RED, true, 0.6f);
            }
            if (enemyPool)
            {
                int gruntCount = GetRandomValue(10, 15);
                for (int g = 0; g < gruntCount; g++)
                {
                    float posX = 0.0f, posY = 0.0f;
                    int edge = GetRandomValue(0, 3);
                    if (edge == 0) { posX = (float)GetRandomValue(0, screenWidth); posY = -25.0f; }
                    else if (edge == 1) { posX = (float)(screenWidth + 25); posY = (float)GetRandomValue(0, screenHeight); }
                    else if (edge == 2) { posX = (float)GetRandomValue(0, screenWidth); posY = (float)(screenHeight + 25); }
                    else { posX = -25.0f; posY = (float)GetRandomValue(0, screenHeight); }
                    enemyPool->Spawn(EnemyType::GRUNT, posX, posY);
                }
            }
            dashTimer_ = (3.0f + (float)GetRandomValue(0, 200) / 100.0f) * 0.7f;
            state_ = 0;
        }
    }

    if (player)
    {
        float dx = playerX - x_;
        float dy = playerY - y_;
        float dist = std::sqrt(dx * dx + dy * dy);
        float minDist = radius_ + playerSize * 0.5f;
        if (dist < minDist && dist > 0.001f)
        {
            dx /= dist;
            dy /= dist;
            float overlap = minDist - dist;
            x_ -= dx * overlap;
            y_ -= dy * overlap;
            const float knockbackStrength = 220.0f;
            player->ApplyKnockback(dx * knockbackStrength, dy * knockbackStrength);
        }
    }
}

bool BossController::TakeDamage(int amount)
{
    if (!active_) return false;
    hitFlashTimer_ = 0.2f;
    health_ -= amount;
    if (health_ <= 0)
    {
        active_ = false;
        return true;
    }
    return false;
}

void BossController::PlayDeathEffect(BulletPool* bulletPool, ParticlePool* particlePool)
{
    if (particlePool) particlePool->SpawnExplosionEffect(x_, y_, 150.0f, 64);
    if (bulletPool)
    {
        for (int p = 0; p < ExplosionProjectileCount; p++)
        {
            float angle = (float)p * (6.28318531f / (float)ExplosionProjectileCount);
            bulletPool->Spawn(x_, y_, std::cos(angle) * explosionSpeed_, std::sin(angle) * explosionSpeed_, ExplosionDamage, 1, false, 16.0f, false, 1.0f, false, false, RED, false, 0.0f, true, 0.5f);
        }
    }
    explosionFlashTimer_ = 0.3f;
}

void BossController::CheckBulletHits(BulletPool& bulletPool, ParticlePool* particlePool, GameManager* gm, XpOrbPool* xpOrbPool)
{
    if (!active_) return;
    for (int j = 0; j < bulletPool.GetPoolSize(); j++)
    {
        Bullet* b = bulletPool.GetBullet(j);
        if (!b || !b->active || b->damagesPlayer) continue;
        float dx = b->position.x - x_, dy = b->position.y - y_;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist < radius_)
        {
            b->Deactivate();
            if (TakeDamage(b->damage))
            {
                PlayDeathEffect(&bulletPool, particlePool);
                if (xpOrbPool) xpOrbPool->SpawnAmount(x_, y_, 10000);
                if (gm) gm->NextWave();
            }
            break;
        }
    }
}

void BossController::TickExplosionFlashTimer(float deltaTime)
{
    if (explosionFlashTimer_ > 0.0f)
    {
        explosionFlashTimer_ -= deltaTime;
        if (explosionFlashTimer_ < 0.0f) explosionFlashTimer_ = 0.0f;
    }
}

void BossController::Draw(int screenWidth, int screenHeight) const
{
    if (!active_) return;
    Color bossColor = MAROON;
    float hft = hitFlashTimer_;
    if (hft > 0.0f)
    {
        const float total = 0.2f;
        float intensity = 0.0f;
        if (hft > total - 0.05f)
            intensity = (total - hft) / 0.05f;
        else if (hft < 0.05f)
            intensity = hft / 0.05f;
        else
            intensity = 1.0f;
        if (intensity > 0.001f)
        {
            bossColor.r = (unsigned char)(bossColor.r + (255 - bossColor.r) * intensity);
            bossColor.g = (unsigned char)(bossColor.g + (255 - bossColor.g) * intensity);
            bossColor.b = (unsigned char)(bossColor.b + (255 - bossColor.b) * intensity);
        }
    }
    DrawCircleV({ x_, y_ }, radius_, bossColor);
    DrawCircleLines((int)x_, (int)y_, radius_, RED);
    float barW = 400.0f;
    float barH = 24.0f;
    float barX = (float)screenWidth * 0.5f - barW * 0.5f;
    float barY = 40.0f;
    DrawRectangle((int)barX, (int)barY, (int)barW, (int)barH, DARKGRAY);
    float fill = (float)health_ / (float)maxHealth_;
    if (fill < 0.0f) fill = 0.0f;
    if (fill > 1.0f) fill = 1.0f;
    DrawRectangle((int)barX, (int)barY, (int)(barW * fill), (int)barH, RED);
    DrawRectangleLines((int)barX, (int)barY, (int)barW, (int)barH, WHITE);
    const char* bossLabel = "BOSS";
    int tw = MeasureText(bossLabel, 20);
    DrawText(bossLabel, (int)((float)screenWidth * 0.5f - tw * 0.5f), (int)(barY - 22), 20, WHITE);
}
