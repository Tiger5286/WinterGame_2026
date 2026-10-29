#pragma once
#include "MutantState.h"
class MutantStateDeath :
    public MutantState
{
public:
    MutantStateDeath(Mutant& mutant);
    ~MutantStateDeath();

    void Enter() override;
    void Update() override;
    void Exit() override;

    ID GetID() const override { return ID::Death; }

private:

};

