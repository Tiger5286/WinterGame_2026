#pragma once
#include "HitboxCollider.h"
#include <string>
#include "Utility/Vector3.h"

class Model;

class HitboxSphere :
    public HitboxCollider
{
public:
    struct Info
    {
        std::wstring frameName;
        float radius = 0.0f;
        Model* pModel = nullptr;
        Vector3 localOffset;
        bool isWeakPoint = false;
    };

public:
    HitboxSphere();
    ~HitboxSphere();

    void Update() override;
    void Draw() override;

    void SetInfo(const Info& info);

private:
    Info m_info;
    Vector3 m_pos;
};

