#include "Player.h"
#include "Resource/ResourceManager.h"
#include "System/PadInput.h"
#include "Game/GameObjects/Camera/Camera.h"

namespace
{
	constexpr float kMaxMoveSpeed = 5.0f;
	constexpr float kMoveAccel = 0.5f;

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

	constexpr const wchar_t* kAnimNames[static_cast<int>(AnimationID::Num)] = {
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
}

Player::Player()
{
}

void Player::Init()
{
	m_physics.Init(&m_transform, m_physics.kDefaultDrag, 0.0f);

	m_pModel = ResourceManager::GetInstance().DuplicateModel(L"PlayerModel");
	m_animator.Init(m_pModel.get());

	for (const auto& name : kAnimNames)
	{
		m_animator.AddAnimation(name);
	}

	m_animator.Play(kAnimNames[static_cast<int>(AnimationID::Idle)]);
}

void Player::Update()
{
	Control();

	m_physics.Update();
	Vector3 velXZ = Vector3(m_physics.m_vel.x, 0.0f, m_physics.m_vel.z);
	if (velXZ.SquaredLength() > kMaxMoveSpeed * kMaxMoveSpeed)
	{
		velXZ = velXZ.Normalized() * kMaxMoveSpeed;
	}
	m_physics.m_vel.x = velXZ.x;
	m_physics.m_vel.z = velXZ.z;

	m_pModel->SetTransform(m_transform);

	m_animator.Update();
}

void Player::Draw() const
{
	m_pModel->Draw();
}

void Player::Control()
{
	// スティック入力を取得
	Vector2 stick = PadInput::GetInstance().GetStickInput(PadInput::LR::Left);

	// カメラの方向を入力と合成
	float cameraRotY = m_pCamera.lock()->GetTransform().rot.y;
	Matrix4x4 rot = Matrix4x4::GetRotY(cameraRotY);
	Vector3 stickVec3 = Vector3(stick.x, 0.0f, stick.y);
	stickVec3 *= rot;

	// 入力方向に移動
	m_physics.m_accel.x = stickVec3.x * kMoveAccel;
	m_physics.m_accel.z = stickVec3.z * kMoveAccel;

	// 入力があるときだけプレイヤーの向きを変える
	if (stickVec3.SquaredLength() > 0.0f)
	{
		float rot = atan2(-stickVec3.z, stickVec3.x) - DX_PI_F / 2;
		m_transform.rot.y = rot;
		// 入力があるときだけ歩くアニメーション
		m_animator.Play(kAnimNames[static_cast<int>(AnimationID::Jog)]);
	}
	else
	{
		// 入力がなければ待機アニメーション
		m_animator.Play(kAnimNames[static_cast<int>(AnimationID::Idle)]);
	}
}