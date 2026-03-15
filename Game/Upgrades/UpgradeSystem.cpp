#include "UpgradeSystem.h"
#include "../../GameManager.h"
#include "raylib.h"
#include <algorithm>

UpgradeSystem::UpgradeSystem() : offerCount_(0)
{
    offers_[0] = -1;
    offers_[1] = -1;
    offers_[2] = -1;
}

int UpgradeSystem::CheckLevelUp(GameManager& gm)
{
    if (gm.GetXp() < gm.GetXpToNextLevel())
        return 0;
    gm.LevelUp();
    return (gm.GetLevel() % 5 == 0) ? 2 : 1;
}

std::vector<int> UpgradeSystem::GetAvailableUpgrades(bool forStart) const
{
    std::vector<int> list;
    if (forStart)
    {
        list.push_back(0); list.push_back(1); list.push_back(2);
        list.push_back(3); list.push_back(4); list.push_back(5);
        list.push_back(6); list.push_back(9); list.push_back(15);
        return list;
    }
    list.push_back(0); list.push_back(1); list.push_back(2);
    list.push_back(5); list.push_back(9); list.push_back(15);
    if (GameManager::Instance().GetShotgunPelletCount() < 25) list.push_back(17);
    list.push_back(18);
    if (!GameManager::Instance().HasRicochetUpgrade()) list.push_back(3);
    else { list.push_back(13); list.push_back(30); }
    if (!GameManager::Instance().HasDeathExplosionUpgrade())
        list.push_back(6);
    else
        { list.push_back(7); if (GameManager::Instance().GetExplosionChancePercent() < 12.5f) list.push_back(8); }
    if (!GameManager::Instance().HasDashUpgrade())
        list.push_back(10);
    else
        { list.push_back(11); list.push_back(12); }
    if (GameManager::Instance().GetMaxDashCharges() < 4)
        list.push_back(16);
    if (!GameManager::Instance().HasAutoAttackerUpgrade()) list.push_back(19);
    else list.push_back(20);
    list.push_back(21);
    if (GameManager::Instance().GetCompanionCount() == 0) list.push_back(22);
    else { list.push_back(23); if (GameManager::Instance().GetCompanionCount() < 2) list.push_back(26); }
    if (GameManager::Instance().GetOrbCompanionCount() == 0) list.push_back(24);
    else if (GameManager::Instance().GetOrbCompanionCount() < 2) list.push_back(27);
    if (GameManager::Instance().GetShotgunCompanionCount() == 0) list.push_back(28);
    else { if (GameManager::Instance().GetShotgunCompanionCount() < 2) list.push_back(29); }
    return list;
}

int UpgradeSystem::GetUpgradeWeight(int id) const
{
    switch (id)
    {
    case 0: case 1: case 2: case 5: case 9: case 15: return 3;
    case 3: case 4: case 6: case 10: case 17: case 18: case 19: case 21: case 22: case 24: case 28: return 2;
    case 7: case 8: case 11: case 12: case 13: case 14: case 16: case 20: case 23: case 26: case 27: case 29: case 30: return 1;
    default: return 1;
    }
}

void UpgradeSystem::FillRandomOffers(bool forStart)
{
    std::vector<int> avail = GetAvailableUpgrades(forStart);
    int n = (int)avail.size();
    offerCount_ = n < 3 ? n : 3;
    if (offerCount_ <= 0) return;
    std::vector<int> weighted;
    for (int id : avail)
    {
        int w = GetUpgradeWeight(id);
        for (int k = 0; k < w; k++) weighted.push_back(id);
    }
    int wn = (int)weighted.size();
    for (int i = wn - 1; i > 0; i--)
    {
        int j = GetRandomValue(0, i);
        if (j != i) { int t = weighted[i]; weighted[i] = weighted[j]; weighted[j] = t; }
    }
    int chosen = 0;
    bool used[31] = {};
    for (int i = 0; i < wn && chosen < offerCount_; i++)
    {
        int id = weighted[i];
        if (id >= 0 && id < 31 && !used[id])
        {
            used[id] = true;
            offers_[chosen++] = id;
        }
    }
}

int UpgradeSystem::GetOffer(int index) const
{
    if (index < 0 || index >= MaxOffers) return -1;
    return offers_[index];
}

void UpgradeSystem::ApplyUpgrade(int id)
{
    switch (id)
    {
    case 0: GameManager::Instance().AddMaxHealth(50); break;
    case 1: GameManager::Instance().AddDamageMultiplier(0.25f); break;
    case 2: GameManager::Instance().AddFireRateMultiplier(0.25f); break;
    case 3: if (!GameManager::Instance().HasRicochetUpgrade()) GameManager::Instance().SetRicochetUpgrade(true); break;
    case 4: if (!GameManager::Instance().HasPierceUpgrade()) GameManager::Instance().SetPierceUpgrade(true); break;
    case 5: GameManager::Instance().AddBulletSizeMultiplier(0.1f); break;
    case 6: if (!GameManager::Instance().HasDeathExplosionUpgrade()) GameManager::Instance().SetDeathExplosionUpgrade(true); break;
    case 7: GameManager::Instance().AddExplosionRadiusMultiplier(0.05f); break;
    case 8: GameManager::Instance().AddExplosionChancePercent(2.5f); break;
    case 9: GameManager::Instance().AddPlayerSpeedMultiplier(0.25f); break;
    case 10: if (!GameManager::Instance().HasDashUpgrade()) GameManager::Instance().SetDashUpgrade(true); break;
    case 11: GameManager::Instance().ReduceDashCooldown(0.5f); break;
    case 12: GameManager::Instance().AddDashLength(30.0f); break;
    case 13: GameManager::Instance().SetRicochetPlusPlusUpgrade(true); break;
    case 14: GameManager::Instance().AddPiercePlus(); break;
    case 30: GameManager::Instance().AddRicochetExtraBounces(2); break;
    case 15: GameManager::Instance().AddOrbMagnetismMultiplier(0.25f); break;
    case 16: GameManager::Instance().AddMaxDashCharges(); break;
    case 17: GameManager::Instance().AddShotgunPelletCount(); break;
    case 18: GameManager::Instance().AddShotgunPelletDamageMultiplier(0.25f); break;
    case 19: if (!GameManager::Instance().HasAutoAttackerUpgrade()) GameManager::Instance().SetAutoAttackerUpgrade(true); break;
    case 20: GameManager::Instance().AddAutoAttackerRange(80.0f); break;
    case 21: GameManager::Instance().AddFollowGunSpeedMultiplier(0.2f); break;
    case 22: if (!GameManager::Instance().HasCompanionUpgrade()) GameManager::Instance().SetCompanionUpgrade(true); break;
    case 23: GameManager::Instance().AddCompanionDamageMultiplier(0.25f); GameManager::Instance().AddCompanionFireRateMultiplier(0.25f); break;
    case 24: if (!GameManager::Instance().HasOrbCompanionUpgrade()) GameManager::Instance().SetOrbCompanionUpgrade(true); break;
    case 26: GameManager::Instance().AddCompanion(); break;
    case 27: GameManager::Instance().AddOrbCompanion(); break;
    case 28: if (!GameManager::Instance().HasShotgunCompanionUpgrade()) GameManager::Instance().SetShotgunCompanionUpgrade(true); break;
    case 29: GameManager::Instance().AddShotgunCompanion(); break;
    default: break;
    }
}

const char* UpgradeSystem::GetUpgradeName(int id) const
{
    switch (id)
    {
    case 0: return "+50 Max Health (+50 heal)";
    case 1: return "+25% Bullet Damage (Pistol)";
    case 2: return "+25% Fire Rate";
    case 3: return "Ricochet: Pistol bounces to 2nd enemy";
    case 4: return "Pierce: Pistol passes through enemies";
    case 5: return "+10% Bullet Size (+10% knockback)";
    case 6: return "Death Explosion (5% chance on kill)";
    case 7: return "+5% Death Explosion radius";
    case 8: return "+2.5% Death Explosion chance (max 12.5%)";
    case 9: return "+25% Move Speed";
    case 10: return "Dash (Space – i-frames, damages enemies)";
    case 11: return "Dash cooldown -0.5s";
    case 12: return "Dash length +30 px";
    case 13: return "Ricochet++: bullet splits to 2 enemies";
    case 14: return "Pierce++: +1 pierce, +10% damage (Pistol)";
    case 15: return "+25% XP orb magnetism range";
    case 16: return "Dash charges +1 (max 3)";
    case 17: return "Shotgun +5 pellets (max 3 upgrades)";
    case 18: return "+25% Shotgun pellet damage";
    case 19: return "Auto Attacker: shoots closest enemy in range";
    case 20: return "Auto Attacker +80 range";
    case 21: return "+20% Pistol projectile speed";
    case 22: return "Pistol Companion (follows, shoots enemies)";
    case 23: return "Companion +25% damage & fire rate";
    case 24: return "Orb Companion (collects XP orbs)";
    case 25: return "Orb Companion +Magnetism & Speed";
    case 26: return "Pistol Companion +1 (2nd)";
    case 27: return "Orb Companion +1 (2nd)";
    case 28: return "Shotgun Companion (weak shotgun)";
    case 29: return "Shotgun Companion +1 (2nd)";
    case 30: return "Ricochet +2 bounces (stackable)";
    default: return "???";
    }
}
