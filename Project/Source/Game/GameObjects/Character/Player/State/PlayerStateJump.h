#pragma once
#include "PlayerState.h"
class PlayerStateJump :
    public PlayerState
{
public:
    PlayerStateJump(Player& player);

    void Enter() override;
    void Update() override;
    void Exit() override;

    ID GetID() const override;
};

