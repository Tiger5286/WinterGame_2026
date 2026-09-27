#pragma once
#include "Component.h"
#include <memory>
#include <functional>

class Collider;

class Collidable :
    public Component
{
public:
    Collidable() = default;
    ~Collidable() = default;

    // OnCollision時に呼ばれる関数を設定する
    void SetFunc(std::function<void(Collidable& other)> func) { m_func = func; }

    // 当たり判定を設定/取得する
    void SetCollider(std::unique_ptr<Collider> pCollider);
    Collider* GetCollider() const { return m_pCollider.get(); }

    // 他のオブジェクトと当たったときにCollisionManagerが呼ぶ
    void OnCollision(Collidable& other) { if (m_func) m_func(other); }

private:
    // 当たり判定
    std::unique_ptr<Collider> m_pCollider;
    // 当たったときに呼ばれる関数
    std::function<void(Collidable& other)> m_func;
};

