#pragma once
#include "PlayerState.h"
#include "Utility/Vector3.h"

class PlayerStateDodge :
    public PlayerState
{
public:
    PlayerStateDodge(Player& player);
    ~PlayerStateDodge();

    void Enter() override;
    void Update() override;
    void Exit() override;

    ID GetID() const override { return ID::Dodge; }

private:
    int m_frame = 0;
    Vector3 m_dir;
};

