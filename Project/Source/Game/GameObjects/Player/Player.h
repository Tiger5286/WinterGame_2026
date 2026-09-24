#pragma once
#include "Game/GameObjects/GameObject.h"

class Model;
class Camera;

class Player :
    public GameObject
{
public:
    Player() = default;
    ~Player() override = default;

    void Init() override;
    void Update() override;
    void Draw() const override;

    void SetCamera(std::weak_ptr<Camera> pCamera) { m_pCamera = pCamera; }

private:
    std::unique_ptr<Model> m_pModel;
    std::weak_ptr<Camera> m_pCamera;
};