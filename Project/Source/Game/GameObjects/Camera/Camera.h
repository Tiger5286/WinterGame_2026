#pragma once
#include "../GameObject.h"

class Player;

class Camera :
    public GameObject
{
public:
    static constexpr float kDefaultFov = 3.141592f / 3.0f;

    Camera() = default;
    ~Camera() override = default;

    void Init() override;
    void Update() override;
    void Draw() const override;

    void SetPlayer(std::weak_ptr<Player> pPlayer) { m_pPlayer = pPlayer; }

private:
    void Control();

private:
    float m_fov = kDefaultFov;
    Vector3 m_targetPos = Vector3::Zero();
    std::weak_ptr<Player> m_pPlayer;
};

