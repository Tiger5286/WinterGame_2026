#pragma once
#include "../GameObject.h"
class Barrier :
    public GameObject
{
public:
    Barrier();
    ~Barrier();

    void Init() override;
    void Update() override;
    void Draw() override;

private:
    void Destroy();

private:
    Model* m_pModel = nullptr;
};

