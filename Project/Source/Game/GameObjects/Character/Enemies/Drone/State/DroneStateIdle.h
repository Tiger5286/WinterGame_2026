#pragma once
#include "DroneState.h"

// 何もしないステート。プレイヤーが見えたらShootに移る
class DroneStateIdle :
    public DroneState
{
public:
    DroneStateIdle(Drone& drone);
    ~DroneStateIdle();

    void Enter() override;
    void Update() override;
    void Exit() override;

    ID GetID() const override { return ID::Idle; }
};
