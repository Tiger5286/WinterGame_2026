#pragma once
#include "MutantState.h"
#include "Utility/Vector3.h"

// バックステップ
class MutantStateBackStep :
    public MutantState
{
public:
    enum class NextState
    {
        Orbit,
        Missile,
    };

public:
    MutantStateBackStep(Mutant& mutant,NextState next = NextState::Orbit);
    ~MutantStateBackStep();

    void Enter() override;
    void Update() override;
    void Exit() override;

    ID GetID() const override { return ID::BackStep; }

private:
	const NextState m_nextState;
	int m_frameCount = 0;
	Vector3 m_backStepDir = Vector3::Zero();
};
