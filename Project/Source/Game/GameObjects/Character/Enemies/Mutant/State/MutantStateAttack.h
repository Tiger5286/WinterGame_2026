#pragma once
#include "MutantState.h"

class Attack;

class MutantStateAttack :
    public MutantState
{
public:
    MutantStateAttack(Mutant& mutant);
    ~MutantStateAttack();

    void Enter() override;
    void Update() override;
    void Exit() override;
    void Draw() override;

    ID GetID() const override { return ID::Attack; }

private:
    float m_frame = 0.0f;
    float m_prevFrame = 0.0f;

    std::shared_ptr<Attack> m_pAtk = nullptr;
};

