#pragma once
#include "MutantState.h"

// プレイヤーの周りをまわる
class MutantStateOrbit :
    public MutantState
{
public:
    MutantStateOrbit(Mutant& mutant);
    ~MutantStateOrbit();

    void Enter() override;
    void Update() override;
    void Exit() override;

    ID GetID() const override { return ID::Orbit; }

private:

};
