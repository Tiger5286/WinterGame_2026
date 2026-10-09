#pragma once
#include "../GameObject.h"
class Character :
    public GameObject
{
public:
    Character();
    virtual ~Character();

    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;

protected:
    int m_hp = 0;
};

