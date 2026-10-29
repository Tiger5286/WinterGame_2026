#pragma once
#include "PlayerState.h"
class PlayerStateHit :
    public PlayerState
{
public:
    PlayerStateHit(Player& player);
    ~PlayerStateHit();

    void Enter() override;
    void Update() override;
    void Exit() override;

    ID GetID() const override { return ID::Hit; }

    bool CanSplitBody() const override { return false; }

private:

};

