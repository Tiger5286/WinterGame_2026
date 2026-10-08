#pragma once
#include "Game/GameObjects/GameObject.h"

class Model;
class Camera;
class HackingManager;
class GameObjectManager;

class Player :
    public GameObject
{
public:
	enum class AnimationID
	{
		Idle,
		Jog,
		Run,
		Jump,
		Fall,
		Land,
		Hover,
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
		L"Player|Jump",
		L"Player|Fall",
		L"Player|Land",
		L"Player|Hover",
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

public:
    Player();
    ~Player() override;

    void Init() override;
    void Update() override;
    void Draw() override;

    void SetCamera(std::weak_ptr<Camera> pCamera) { m_pCamera = pCamera; }
	void SetHackingManager(std::weak_ptr<HackingManager> pHackingManager) { m_pHackingManager = pHackingManager; }
	void SetGameObjectManager(std::weak_ptr<GameObjectManager> pGameObjectManager) { m_pGameObjectManager = pGameObjectManager; }


    bool IsAim() const { return m_isAim; }

private:	// プレイヤーだけが使う関数
	void UpdateAnimation();
	void UpdateAim();
	std::shared_ptr<GameObject> FindNearestVisibleEnemy();

private:
    std::unique_ptr<Model> m_pModel;
    std::weak_ptr<Camera> m_pCamera;
	std::weak_ptr<HackingManager> m_pHackingManager;
	std::weak_ptr<GameObjectManager> m_pGameObjectManager;
    float m_angle = 0.0f;

    bool m_isAim = false;
	bool m_isRun = false;

	float m_aimStartAngle = 0.0f;
	int m_aimStartFrame = 0;

	std::unique_ptr<Model> m_pGunModel;
	int m_haveGunFrameIndex = -1;

	// プレイヤーのステートがプレイヤーの状態を変更したいときもあるためfriend
	// PlayerStateがPlayerのどのメンバを使っていいか判定する
	friend class PlayerState;
};