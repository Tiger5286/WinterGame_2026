#pragma once
#include "Components/State/State.h"

class Walker;
class Player;

class WalkerState :
    public State<Walker>
{
public:
    static constexpr float kAccel = 0.15f;
    static constexpr float kMinDistance = 100.0f;

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

