#pragma once
#include "DroneState.h"

// 撃破されたときのステート
// アニメーションを止めて落下し、床に着いたら爆発して消える
class DroneStateFall :
    public DroneState
{
public:
    DroneStateFall(Drone& drone);
    ~DroneStateFall();

    void Enter() override;
    void Update() override;
    void Exit() override;

    ID GetID() const override { return ID::Fall; }

private:
    // 爆発エフェクトを出して消える
    void Explode();

private:
    float m_fallSpeed = 0.0f;
    float m_frame = 0.0f;
};
