#pragma once
#include "PlayerState.h"
class PlayerStateDeath :
    public PlayerState
{
public:
    PlayerStateDeath(Player& player);
    ~PlayerStateDeath();

    void Enter() override;
    void Update() override;
    void Exit() override;

    ID GetID() const override { return ID::Death; }

    bool CanSplitBody() const override { return false; }

private:

};

