#pragma once
#include "MutantState.h"

// 突進攻撃
class MutantStateCharge :
    public MutantState
{
public:
    MutantStateCharge(Mutant& mutant);
    ~MutantStateCharge();

    void Enter() override;
    void Update() override;
    void Exit() override;

    ID GetID() const override { return ID::Charge; }

private:

};
