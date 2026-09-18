#pragma once
#include "Game/GameObject.h"
class Camera :
    public GameObject
{
public:
    Camera() = default;
    ~Camera() override = default;

    void Init() override;
    void Update() override;
    void Draw() const override;

private:

};

