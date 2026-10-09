#pragma once
#include "UIBase.h"
#include "Utility/Vector2.h"
class HackingTargetUI :
    public UIBase
{
public:
    struct Info
    {
        Vector2 screenPos;
    };

public:
    HackingTargetUI();
    ~HackingTargetUI();

    void Init() override;
    void Update() override;
    void Draw() override;

    void SetInfo(const Info& info) { m_info = info; }

private:
    Info m_info;
};

