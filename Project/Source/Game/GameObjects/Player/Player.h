#pragma once
#include "Game/GameObjects/GameObject.h"

class Model;
class Camera;

class Player :
    public GameObject
{
public:
	enum class AnimationID
	{
		Idle,
		Jog,
		Run,
		AimIdle,
		AimFire,
		AimWalkForward,
		AimWalkBackward,
		AimWalkLeft,
		AimWalkRight,
		AimWalkForwardLeft,
		AimWalkForwardRight,
		AimWalkBackwardLeft,
		AimWalkBackwardRight,

		Num
	};

	static constexpr const wchar_t* kAnimNames[static_cast<int>(AnimationID::Num)] = {
		L"Player|Idle",
		L"Player|Jog",
		L"Player|Run",
		L"Player|AimIdle",
		L"Player|AimFire",
		L"Player|AimWalkForward",
		L"Player|AimWalkBackward",
		L"Player|AimWalkLeft",
		L"Player|AimWalkRight",
		L"Player|AimWalkForwardLeft",
		L"Player|AimWalkForwardRight",
		L"Player|AimWalkBackwardLeft",
		L"Player|AimWalkBackwardRight"
	};

	static_assert(static_cast<int>(AnimationID::Num) == std::size(kAnimNames));

public:
    Player();
    ~Player() override = default;

    void Init() override;
    void Update() override;
    void Draw() override;

    void SetCamera(std::weak_ptr<Camera> pCamera) { m_pCamera = pCamera; }

    bool IsAim() const { return m_isAim; }

private:
    std::unique_ptr<Model> m_pModel;
    std::weak_ptr<Camera> m_pCamera;
    float m_angle = 0.0f;

    bool m_isAim = false;

    // コンポーネント
    //Physics m_physics;
    //Animator m_animator;
    //StateMachine<Player> m_stateMachine;
};