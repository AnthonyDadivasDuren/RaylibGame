#pragma once

#include <vector>
#include <string>

class GameManager;

class UpgradeSystem
{
public:
    static const int MaxOffers = 3;

    UpgradeSystem();

    void FillRandomOffers(bool forStart);

    int CheckLevelUp(GameManager& gm);

    int GetOfferCount() const { return offerCount_; }
    int GetOffer(int index) const;

    void ApplyUpgrade(int id);
    const char* GetUpgradeName(int id) const;

private:
    std::vector<int> GetAvailableUpgrades(bool forStart) const;
    int GetUpgradeWeight(int id) const;

    int offers_[MaxOffers];
    int offerCount_;
};
