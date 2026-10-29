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
    int m_frame = 0;

    std::shared_ptr<Attack> m_pAtk = nullptr;
};

