#pragma once
#include "Game/GameObjects/GameObject.h"
#include "Components/Animator/Animator.h"
#include "Components/Physics.h"

class Model;
class Camera;

class Player :
    public GameObject
{
public:
    Player();
    ~Player() override = default;

    void Init() override;
    void Update() override;
    void Draw() const override;

    void SetCamera(std::weak_ptr<Camera> pCamera) { m_pCamera = pCamera; }

private:
    void Control();

private:
    std::unique_ptr<Model> m_pModel;
    std::weak_ptr<Camera> m_pCamera;
    float m_angle = 0.0f;

    // コンポーネント
    Physics m_physics;
    Animator m_animator;
};