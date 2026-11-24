#pragma once
#include "Game/GameObjects/Character/Character.h"
#include "Utility/Vector2.h"
#include "Components/Hackable.h"
#include <deque>

class Model;
class Camera;
class HackingManager;
class GameObjectManager;
class HackingTargetUI;
class PlayerInfoUI;

class Player :
    public Character
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
		DodgeForward,
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
		Hit,
		Death,

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
		L"Player|DodgeForward",
		L"Player|AimIdle",
		L"Player|AimFire",
		L"Player|AimWalkForward",
		L"Player|AimWalkBackward",
		L"Player|AimWalkLeft",
		L"Player|AimWalkRight",
		L"Player|AimWalkForwardLeft",
		L"Player|AimWalkForwardRight",
		L"Player|AimWalkBackwardLeft",
		L"Player|AimWalkBackwardRight",
		L"Player|HitReact",
		L"Player|Death"
	};

	// 持てる特殊ノードの種類の上限
	static constexpr int kCanHaveSpecialNodeTypeMax = 2;
	// 持てる特殊ノードの種類ごとの上限
	static constexpr int kCanHaveSpecialNodeNumMax = 2;

public:
    Player();
    ~Player() override;

    void Init() override;
    void Update() override;
    void Draw() override;

	void OnHitAttack(const CollisionManager::AttackInfo& info) override;

    void SetCamera(std::weak_ptr<Camera> pCamera) { m_pCamera = pCamera; }
	void SetHackingManager(std::weak_ptr<HackingManager> pHackingManager) { m_pHackingManager = pHackingManager; }
	void SetGameObjectManager(std::weak_ptr<GameObjectManager> pGameObjectManager) { m_pGameObjectManager = pGameObjectManager; }

    bool IsAim() const { return m_isAim; }

	void SetAmmoMax();

	int GetMaxHP() const;

	int GetAmmo() const { return m_ammo; }
	int GetMaxAmmo() const;

	float GetThruster() const { return m_thruster; }
	float GetMaxThruster() const;

	void SetAngle(float angle);
	void SetDefaultPos(const Vector3& pos);

	void OnActiveTrigger();

	// 特殊ノードを追加する
	bool AddSpecialNode(Hackable::SpecialNode type, int count);
	// 特殊ノードを一回使う
	bool UseSpecialNode(Hackable::SpecialNode type);
	// 盤面に出す特殊ノードの種類
	std::vector<Hackable::SpecialNode> GetSpecialNodeTypes() const;
	// 盤面の更新が必要か判定するための番号
	int GetSpecialNodeVersion() const { return m_specialNodeVersion; }

private:	// プレイヤーだけが使う関数
	void UpdateAnimation();
	void UpdateAim();
	void UpdateThruster();
	// スラスターを消費するときに呼ぶ関数
	void UseThruster(float usedChargeValue);
	/// <summary>
	/// スラスターを消費するステートに遷移する関数。スラスターがないときは遷移しない
	/// </summary>
	/// <returns>true:遷移した / false:スラスターが無いため遷移しなかった</returns>
	bool ChangeStateDodge();
	bool ChangeStateHover();

	// エイム・射撃・ハッキングができる状態か(被弾中・死亡中はできない)
	bool CanAction();
	std::shared_ptr<GameObject> FindNearestVisibleHackable(Vector2& screenPos);

private:
    std::unique_ptr<Model> m_pModel;
    std::weak_ptr<Camera> m_pCamera;
	std::weak_ptr<HackingManager> m_pHackingManager;
	std::weak_ptr<GameObjectManager> m_pGameObjectManager;
	std::shared_ptr<PlayerInfoUI> m_pPlayerInfoUI = nullptr;
    float m_angle = 0.0f;

    bool m_isAim = false;
	bool m_isRun = false;

	float m_aimStartAngle = 0.0f;
	float m_aimStartFrame = 0;

	std::unique_ptr<Model> m_pGunModel;
	int m_haveGunFrameIndex = -1;

	std::shared_ptr<HackingTargetUI> m_pHackingTargetUI = nullptr;

	float m_shotIntervalFrame = 0;
	bool m_isShooting = false;

	float m_thruster;
	// スラスターを最後に使用してから何フレーム経過したか
	float m_thrusterRechargeFrame = 0;

	// 残弾数
	int m_ammo;
	// 最後に弾を撃ってから何フレーム経過したか
	float m_ammoRechargeFrame = 0.0f;
	float m_ammoRechargeInterval = 0.0f;

	float m_triggerFrame = 0.0f;

	// 初期位置
	Vector3 m_defaultPos;

	// 特殊ノード関係
	// 盤面のバージョン
	int m_specialNodeVersion = 0;	// 盤面に出る種類が変わるたびに+1
	// 特殊ノードの種類と所持数を意味する構造体
	struct HaveSpecialNode
	{
		Hackable::SpecialNode type = Hackable::SpecialNode::None;
		int num = 0;
	};
	// 所持している特殊ノード
	std::deque<HaveSpecialNode> m_specialNodes;

	// プレイヤーのステートがプレイヤーの状態を変更したいときもあるためfriend
	// PlayerStateがPlayerのどのメンバを使っていいか判定する
	friend class PlayerState;
};