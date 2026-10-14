#pragma once
#include "WalkerState.h"

class Attack;

class WalkerStateAttack :
    public WalkerState
{
public:
    WalkerStateAttack(Walker& walker);
    ~WalkerStateAttack();

    void Enter() override;
    void Update() override;
    void Exit() override;
    void Draw() override;

    ID GetID() const override { return ID::Attack; }

private:
    int m_frame = 0;

    std::shared_ptr<Attack> m_pAtk = nullptr;
};

