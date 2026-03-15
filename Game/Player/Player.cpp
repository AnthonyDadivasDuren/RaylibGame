#include "Player.h"
#include "raylib.h"
#include <cmath>

Player::Player()
    : positionX_(900.0f)
    , positionY_(675.0f)
    , size_(50.0f)
    , speed_(300.0f)
    , knockbackX_(0.0f)
    , knockbackY_(0.0f)
    , lastMoveDirX_(1.0f)
    , lastMoveDirY_(0.0f)
    , invincibleTimer_(0.0f)
    , hitFlashTimer_(0.0f)
    , hitInvincibleTimer_(0.0f)
{
}

void Player::SetPosition(float x, float y)
{
    positionX_ = x;
    positionY_ = y;
}

void Player::AddPosition(float dx, float dy)
{
    positionX_ += dx;
    positionY_ += dy;
}

void Player::ApplyKnockback(float dx, float dy)
{
    knockbackX_ += dx;
    knockbackY_ += dy;
}

void Player::SetInvincibleTimer(float t) { invincibleTimer_ = t; }
void Player::SetHitFlashTimer(float t) { hitFlashTimer_ = t; }
void Player::SetHitInvincibleTimer(float t) { hitInvincibleTimer_ = t; }

void Player::UpdateTimers(float deltaTime)
{
    if (invincibleTimer_ > 0.0f)
    {
        invincibleTimer_ -= deltaTime;
        if (invincibleTimer_ < 0.0f) invincibleTimer_ = 0.0f;
    }
    if (hitFlashTimer_ > 0.0f)
    {
        hitFlashTimer_ -= deltaTime;
        if (hitFlashTimer_ < 0.0f) hitFlashTimer_ = 0.0f;
    }
    if (hitInvincibleTimer_ > 0.0f)
    {
        hitInvincibleTimer_ -= deltaTime;
        if (hitInvincibleTimer_ < 0.0f) hitInvincibleTimer_ = 0.0f;
    }
}

void Player::UpdateMovement(float deltaTime, float speedMultiplier, int screenWidth, int screenHeight)
{
    float moveSpeed = speed_ * speedMultiplier * deltaTime;
    float dx = 0.0f, dy = 0.0f;
    if (IsKeyDown(KEY_W)) dy -= 1.0f;
    if (IsKeyDown(KEY_S)) dy += 1.0f;
    if (IsKeyDown(KEY_A)) dx -= 1.0f;
    if (IsKeyDown(KEY_D)) dx += 1.0f;
    if (dx != 0.0f || dy != 0.0f)
    {
        float len = std::sqrt(dx * dx + dy * dy);
        if (len > 0.001f) { dx /= len; dy /= len; }
        lastMoveDirX_ = dx;
        lastMoveDirY_ = dy;
        positionX_ += dx * moveSpeed + knockbackX_ * deltaTime;
        positionY_ += dy * moveSpeed + knockbackY_ * deltaTime;
    }
    else
    {
        positionX_ += knockbackX_ * deltaTime;
        positionY_ += knockbackY_ * deltaTime;
    }
    knockbackX_ *= 0.88f;
    knockbackY_ *= 0.88f;
    if (positionX_ < 0.0f) positionX_ = 0.0f;
    if (positionX_ > (float)(screenWidth - (int)size_)) positionX_ = (float)(screenWidth - (int)size_);
    if (positionY_ < 0.0f) positionY_ = 0.0f;
    if (positionY_ > (float)(screenHeight - (int)size_)) positionY_ = (float)(screenHeight - (int)size_);
}

void Player::Reset(float startX, float startY)
{
    positionX_ = startX;
    positionY_ = startY;
    knockbackX_ = 0.0f;
    knockbackY_ = 0.0f;
    lastMoveDirX_ = 1.0f;
    lastMoveDirY_ = 0.0f;
    invincibleTimer_ = 0.0f;
    hitFlashTimer_ = 0.0f;
    hitInvincibleTimer_ = 0.0f;
}
