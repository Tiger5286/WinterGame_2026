#pragma once
#include "MutantState.h"
class MutantStateApproach :
    public MutantState
{
public:
    MutantStateApproach(Mutant& mutant);
    ~MutantStateApproach();

    void Enter() override;
    void Update() override;
    void Exit() override;

    ID GetID() const override { return ID::Approach; }

private:
	int m_approachTime = 0;	// 接近してからの経過時間
};

