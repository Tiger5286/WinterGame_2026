#pragma once
#include "PlayerState.h"
class PlayerStateHover :
    public PlayerState
{
    public:
    PlayerStateHover(Player& player) :
        PlayerState(player)
    {
    }

    virtual ~PlayerStateHover() = default;

    void Enter() override;
    void Update() override;
    void Exit() override;

	ID GetID() const override { return ID::Hover; }

private:
    void KeepVelY();

private:

};

