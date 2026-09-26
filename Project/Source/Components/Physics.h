#pragma once
#include "Component.h"
#include "Utility/Vector3.h"

class Transform;

class Physics :
    public Component
{
public:
    static constexpr float kDefaultDrag = 0.9f;
    static constexpr float kDefaultGravity = -1.0f;
    static constexpr float kDefaultMaxSpeed = 30.0f;

    void Init(Transform* pTransform,float drag = kDefaultDrag,float gravity = kDefaultGravity);
    void Update();

    // 速度の減衰率を設定する。0.0~1.0の範囲外の数値を入れるとクランプされる。
    void SetDrag(float drag);
    float GetDrag() const { return m_drag; }

    void SetGravity(float gravity) { m_gravity = gravity; }
    float GetGravity() const { return m_gravity; }

    void SetMaxSpeed(float maxSpeed) { m_maxSpeed = maxSpeed; }
    float GetMaxSpeed() const { return m_maxSpeed; }

    /// <summary>
    /// xz成分だけの速度の2乗を返す
    /// </summary>
    /// <returns>xz成分だけの速度の2乗</returns>
    float GetSquaredMoveSpeed() const { return Vector3(m_vel.x,0.0f, m_vel.z).SquaredLength(); }

    Vector3 m_vel;  // posに毎フレーム足す速度
    Vector3 m_accel;    // velに毎フレーム足す加速度

private:
    Transform* m_pTransform = nullptr;  // Transformの参照

    float m_drag = kDefaultDrag;    // velに毎フレーム掛ける速度の減衰率(0.0~1.0)(1.0にすると減衰しない)(XZ成分にのみ有効)
    float m_gravity = -kDefaultGravity;    // vel.yに毎フレーム足す重力加速度(0にすると重力なし)
    // 最高速度(XZ成分にのみ有効)
    float m_maxSpeed = kDefaultMaxSpeed;
};