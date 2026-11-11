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

    int GetHP() const { return m_hp; }

protected:
    int m_hp = 0;
};

