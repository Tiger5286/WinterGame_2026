#pragma once
#include "UIBase.h"
#include "Utility/Vector3.h"

class EnemyHPUI :
    public UIBase
{
public:
    struct Info
    {
        Vector3 uiPos;
        int maxHP = 0;
        int nowHP = 0;
    };

public:
    EnemyHPUI();
    ~EnemyHPUI();

    void Init() override;
    void Update() override;
    void Draw() override;

    void SetInfo(const Info& info) { m_info = info; }

private:
    Info m_info;
};

