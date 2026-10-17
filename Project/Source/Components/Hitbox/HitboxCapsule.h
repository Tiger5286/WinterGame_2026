#pragma once
#include "HitboxCollider.h"
#include <string>
#include "Utility/Vector3.h"

class Model;

class HitboxCapsule :
    public HitboxCollider
{
public:
    struct Info
    {
        std::wstring bottomFrameName;
        std::wstring topFrameName;
        float radius = 0.0f;
        Model* pModel = nullptr;
        Vector3 bottomLocalOffset;
        Vector3 topLocalOffset;
        bool isWeakPoint = false;
    };

public:
    HitboxCapsule();
    ~HitboxCapsule();

    void Update() override;
    void Draw() override;

    void SetInfo(const Info& info) { m_info = info; }
    const Info& GetInfo() const { return m_info; }

    const Type GetType() const override { return Type::Capsule; }

    const Vector3& GetBottom() const { return m_bottomPos; }
    const Vector3& GetTop() const { return m_topPos; }

private:
    Info m_info;
    Vector3 m_bottomPos;
    Vector3 m_topPos;
};

