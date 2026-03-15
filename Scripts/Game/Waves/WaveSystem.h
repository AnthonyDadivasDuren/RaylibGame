#pragma once

class WaveSystem
{
public:
    WaveSystem();

    void SetDuration(float seconds);
    float GetDuration() const { return duration_; }
    float GetTimer() const { return timer_; }
    float GetTimeRemaining() const { return duration_ - timer_; }

    void AddTime(float deltaTime);
    bool IsComplete() const { return timer_ >= duration_; }
    void ResetTimer();

    void Reset();

private:
    float timer_;
    float duration_;
};
