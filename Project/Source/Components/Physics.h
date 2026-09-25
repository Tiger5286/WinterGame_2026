#pragma once
#include "Component.h"
#include "Utility/Vector3.h"

class Transform;

class Physics :
    public Component
{
public:
    void Init(Transform* pTransform);
    void Update();

    // 速度の減衰率を設定する。0.0~1.0の範囲外の数値を入れるとクランプされる。
    void SetDrag(float drag);
    float GetDrag() const { return m_drag; }

    void SetGravity(float gravity) { m_gravity = gravity; }
    float GetGravity() const { return m_gravity; }

    Vector3 m_vel;  // posに毎フレーム足す速度
    Vector3 m_accel;    // velに毎フレーム足す加速度

private:
    Transform* m_pTransform = nullptr;  // Transformの参照

    float m_drag = 0.9f;    // velに毎フレーム掛ける速度の減衰率(0.0~1.0)(1.0にすると減衰しない)
    float m_gravity = -1.0f;    // vel.yに毎フレーム足す重力加速度(0にすると重力なし)
};