#pragma once
#include "MutantState.h"

// バックステップ
class MutantStateBackStep :
    public MutantState
{
public:
    MutantStateBackStep(Mutant& mutant);
    ~MutantStateBackStep();

    void Enter() override;
    void Update() override;
    void Exit() override;

    ID GetID() const override { return ID::BackStep; }

private:

};
