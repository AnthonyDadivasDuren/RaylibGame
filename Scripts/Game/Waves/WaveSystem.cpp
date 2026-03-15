#include "Game/Waves/WaveSystem.h"

WaveSystem::WaveSystem() : timer_(0.0f), duration_(60.0f)
{
}

void WaveSystem::SetDuration(float seconds)
{
    duration_ = seconds;
}

void WaveSystem::AddTime(float deltaTime)
{
    timer_ += deltaTime;
}

void WaveSystem::ResetTimer()
{
    timer_ = 0.0f;
}

void WaveSystem::Reset()
{
    timer_ = 0.0f;
}
