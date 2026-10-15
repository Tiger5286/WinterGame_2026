#pragma once
#include "WalkerState.h"
class WalkerStateDeath :
    public WalkerState
{
public:
    WalkerStateDeath(Walker& walker);
    ~WalkerStateDeath();

    void Enter() override;
    void Update() override;
    void Exit() override;

    ID GetID() const override { return ID::Death; }

private:

};

