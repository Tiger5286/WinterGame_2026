#pragma once
#include "GameObject.h"
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

};

