#pragma once
#include "MutantState.h"

// ミサイル攻撃
class MutantStateMissile :
    public MutantState
{
public:
    MutantStateMissile(Mutant& mutant);
    ~MutantStateMissile();

    void Enter() override;
    void Update() override;
    void Exit() override;

    ID GetID() const override { return ID::Missile; }

private:
    int m_frameCount = 0;
};
