#pragma once
#include "MutantState.h"

class Attack;

// 突進攻撃
class MutantStateCharge :
    public MutantState
{
public:
    MutantStateCharge(Mutant& mutant,int chargeCount = 1);
    ~MutantStateCharge();

    void Enter() override;
    void Update() override;
    void Exit() override;
    void Draw() override;

    ID GetID() const override { return ID::Charge; }

private:
    const int m_chargeCount;
    int m_frameCount = 0;
    Vector3 m_toPlayerVec;

    std::shared_ptr<Attack> m_pAtk = nullptr;
};