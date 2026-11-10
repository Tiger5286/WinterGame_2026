#pragma once
#include "MutantState.h"
class MutantStateMoveEdge :
    public MutantState
{
public:
    MutantStateMoveEdge(Mutant& mutant);
    ~MutantStateMoveEdge();

    void Enter() override;
    void Update() override;
    void Exit() override;

    ID GetID() const override { return ID::MoveEdge; }
private:
    Vector3 m_targetPos;
};