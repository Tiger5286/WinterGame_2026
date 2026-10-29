#pragma once
#include "MutantState.h"
class MutantStateInterval :
    public MutantState
{
public:
    MutantStateInterval(Mutant& mutant);
    ~MutantStateInterval();

    void Enter() override;
    void Update() override;
    void Exit() override;

    ID GetID() const override { return ID::Interval; }

private:

};

