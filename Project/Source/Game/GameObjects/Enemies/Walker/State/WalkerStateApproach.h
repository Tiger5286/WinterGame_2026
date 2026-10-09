#pragma once
#include "WalkerState.h"

class Walker;

class WalkerStateApproach :
    public WalkerState
{
public:
    WalkerStateApproach(Walker& walker);
    ~WalkerStateApproach();

    void Enter() override;
    void Update() override;
    void Exit() override;

    ID GetID() const override { return ID::Approach; }
private:

};

