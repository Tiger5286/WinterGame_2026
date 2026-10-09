#pragma once
#include "WalkerState.h"
class WalkerStateOrbit :
    public WalkerState
{
public:
    WalkerStateOrbit(Walker& walker);
    ~WalkerStateOrbit();

    void Enter() override;
    void Update() override;
    void Exit() override;

    ID GetID() const override { return ID::Orbit; }

private:

};

