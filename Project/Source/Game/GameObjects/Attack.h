#pragma once
#include "GameObject.h"
#include <vector>

class SphereCollider;

class Attack :
    public GameObject
{
public:
    struct Data
    {
        CollisionManager::AttackInfo info = {};
        float colRadius = 0.0f;
    };

public:
    Attack();
    ~Attack();

    void Init() override;
    void Update() override;
    void Draw() override;

    // 攻撃に必要な情報を設定する。※Initの前に実行する。
    void SetData(const Data& data);

    void OnCollision(GameObject& other) override;

private:
    std::vector<int> m_hitObjectIds;
    CollisionManager::AttackInfo m_info;
    float m_radius = 0.0f;
};