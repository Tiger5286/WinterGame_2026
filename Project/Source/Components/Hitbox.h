#pragma once
#include "Component.h"
#include <vector>
#include <memory>
#include "Utility/Vector3.h"

class Collider;

class Hitbox : public Component
{
public:
    struct Info
    {
        std::shared_ptr<Collider> pCollider = nullptr;
        bool isWeakPoint = false;
    };

public:
    Hitbox();
    ~Hitbox();

    void Init(std::vector<Info> colDatas);
    void Draw();

    const std::vector<Info>& GetColliders() { return m_colliders; }

private:
    std::vector<Info> m_colliders;
};

