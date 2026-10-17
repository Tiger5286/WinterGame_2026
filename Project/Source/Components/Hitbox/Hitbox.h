#pragma once
#include "Components/Component.h"
#include <vector>
#include <memory>
#include "Utility/Vector3.h"
#include "HitboxCollider.h"
#include <string>
#include "HitboxSphere.h"
#include "HitboxCapsule.h"

class Collider;
class Transform;
class Model;

class Hitbox : public Component
{
public:
    Hitbox();
    ~Hitbox();

    void Update();
    void Draw();

    void AddCollider(const HitboxSphere::Info& info);
    void AddCollider(const HitboxCapsule::Info& info);

    const std::vector<std::shared_ptr<HitboxCollider>>& GetHitboxes() const { return m_hitboxes; }

private:
    std::vector<std::shared_ptr<HitboxCollider>> m_hitboxes;
};