#pragma once

class Player
{
public:
    Player();

    float GetX() const { return positionX_; }
    float GetY() const { return positionY_; }
    float GetCenterX() const { return positionX_ + size_ * 0.5f; }
    float GetCenterY() const { return positionY_ + size_ * 0.5f; }
    float GetSize() const { return size_; }
    float GetSpeed() const { return speed_; }
    float GetLastMoveDirX() const { return lastMoveDirX_; }
    float GetLastMoveDirY() const { return lastMoveDirY_; }
    float GetInvincibleTimer() const { return invincibleTimer_; }
    float GetHitFlashTimer() const { return hitFlashTimer_; }
    float GetHitInvincibleTimer() const { return hitInvincibleTimer_; }

    void SetPosition(float x, float y);
    void AddPosition(float dx, float dy);
    void ApplyKnockback(float dx, float dy);
    void SetInvincibleTimer(float t);
    void SetHitFlashTimer(float t);
    void SetHitInvincibleTimer(float t);

    void UpdateTimers(float deltaTime);
    void UpdateMovement(float deltaTime, float speedMultiplier, int screenWidth, int screenHeight);

    bool IsVulnerable() const { return invincibleTimer_ <= 0.0f && hitInvincibleTimer_ <= 0.0f; }

    void Reset(float startX, float startY);

private:
    float positionX_, positionY_;
    float size_;
    float speed_;
    float knockbackX_, knockbackY_;
    float lastMoveDirX_, lastMoveDirY_;
    float invincibleTimer_;
    float hitFlashTimer_;
    float hitInvincibleTimer_;
};
