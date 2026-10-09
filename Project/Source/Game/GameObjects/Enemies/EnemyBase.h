#pragma once
#include "../GameObject.h"

class EnemyBase :
    public GameObject
{
public:
    EnemyBase();
    virtual ~EnemyBase() override;

    virtual void Init() override = 0;
    virtual void Update() override = 0;
    virtual void Draw() override = 0;



private:
};

