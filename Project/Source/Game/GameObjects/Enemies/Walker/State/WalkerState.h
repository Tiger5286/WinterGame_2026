#pragma once
#include "Components/State/State.h"

class Walker;
class Player;

class WalkerState :
    public State<Walker>
{
public:
    enum class ID
    {
        Approach,
        Orbit,
        // Attack,

        Num
    };

public:
    WalkerState(Walker& walker);
    virtual ~WalkerState();

    virtual void Enter() override = 0;
    virtual void Update() override = 0;
    virtual void Exit() override = 0;

    virtual ID GetID() const = 0;

protected:
    std::shared_ptr<Player> GetPlayer() const;
};

