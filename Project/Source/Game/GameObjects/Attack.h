#pragma once
#include "GameObject.h"
#include <vector>

class Attack :
    public GameObject
{
public:
    Attack();
    ~Attack();

    void Init() override;
    void Update() override;
    void Draw() override;

private:
    std::vector<int> m_hitObjectIds;
};