#include "Player.h"
#include "Resource/ResourceManager.h"
#include "System/PadInput.h"
#include "Game/GameObjects/Camera/Camera.h"
#include "Utility/MyLib.h"
#include "State/PlayerStateIdle.h"

namespace
{
	constexpr float kMaxMoveSpeed = 5.0f;
	constexpr float kMoveAccel = 0.5f;
}

Player::Player() :
	m_stateMachine(*this)
{
}

void Player::Init()
{
	// physicsを初期化
	m_physics.Init(GetComponent<Transform>(), m_physics.kDefaultDrag, 0.0f);

	// モデルを取得してアニメーションを初期化
	m_pModel = ResourceManager::GetInstance().DuplicateModel(L"PlayerModel");
	m_animator.Init(m_pModel.get());
	for (const auto& name : kAnimNames)	// アニメーションを追加
	{
		m_animator.AddAnimation(name);
	}

	// ステートを初期化
	m_stateMachine.ChangeState(std::make_unique<PlayerStateIdle>(*this));
}

void Player::Update()
{
	Transform& transform = *GetComponent<Transform>();

	//Control();

	// ステートを更新
	m_stateMachine.Update();

	// physicsの更新
	m_physics.Update();
	Vector3 velXZ = Vector3(m_physics.m_vel.x, 0.0f, m_physics.m_vel.z);
	if (velXZ.SquaredLength() > kMaxMoveSpeed * kMaxMoveSpeed)
	{
		velXZ = velXZ.Normalized() * kMaxMoveSpeed;
	}
	m_physics.m_vel.x = velXZ.x;
	m_physics.m_vel.z = velXZ.z;

	// 向きを更新
	float diff = MyLib::GetAngleDiff(m_angle, transform.rot.y);
	transform.rot.y += diff * 0.1f;

	// モデルの行列を更新
	m_pModel->SetTransform(transform);

	// アニメーションを更新
	m_animator.Update();
}

void Player::Draw() const
{
	// モデルを描画
	m_pModel->Draw();
	m_stateMachine.Draw();	// ステートに描画したい内容があったら描画
}

//void Player::Control()
//{
//	auto& input = PadInput::GetInstance();
//	// スティック入力を取得
//	Vector2 stick = input.GetStickInput(PadInput::LR::Left);
//
//	// カメラの方向を入力と合成
//	float cameraRotY = m_pCamera.lock()->GetTransform().rot.y;
//	Matrix4x4 rot = Matrix4x4::GetRotY(cameraRotY);
//	Vector3 stickVec3 = Vector3(stick.x, 0.0f, stick.y);
//	stickVec3 *= rot;
//
//	// 入力方向に移動
//	m_physics.m_accel.x = stickVec3.x * kMoveAccel;
//	m_physics.m_accel.z = stickVec3.z * kMoveAccel;
//
//	// 入力があるときだけプレイヤーの向きを変える
//	if (stickVec3.SquaredLength() > 0.0f)
//	{
//		float rot = atan2(-stickVec3.z, stickVec3.x) - DX_PI_F / 2;
//		m_angle = rot;
//		// 入力があるときだけ歩くアニメーション
//		m_animator.Play(kAnimNames[static_cast<int>(AnimationID::Jog)]);
//	}
//	else
//	{
//		// 入力がなければ待機アニメーション
//		m_animator.Play(kAnimNames[static_cast<int>(AnimationID::Idle)]);
//	}
//
//	if (input.IsPressedTrigger(PadInput::LR::Left))
//	{
//		m_isAim = true;
//		m_angle = cameraRotY - DX_PI_F;
//	}
//	else
//	{
//		m_isAim = false;
//	}
//}