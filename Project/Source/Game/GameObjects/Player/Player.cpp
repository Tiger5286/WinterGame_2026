#include "Player.h"
#include "Resource/ResourceManager.h"
#include "System/PadInput.h"
#include "Game/GameObjects/Camera/Camera.h"
#include "Utility/MyLib.h"
#include "State/PlayerStateIdle.h"
#include "Components/Animator/Animator.h"
#include "Components/Physics.h"
#include "Components/State/StateMachine.h"
namespace
{
	constexpr float kMaxMoveSpeed = 5.0f;
	constexpr float kMoveAccel = 0.5f;
}

Player::Player()
{
	AddComponent<Animator>();
	AddComponent<Physics>();
	AddComponent<StateMachine<Player>>(*this);
}

void Player::Init()
{
	// physicsを初期化
	GetComponent<Physics>()->Init(GetComponent<Transform>(), Physics::kDefaultDrag, 0.0f);

	// モデルを取得してアニメーションを初期化
	m_pModel = ResourceManager::GetInstance().DuplicateModel(L"PlayerModel");
	auto animator = GetComponent<Animator>();
	animator->Init(m_pModel.get());
	for (const auto& name : kAnimNames)	// アニメーションを追加
	{
		animator->AddAnimation(name);
	}

	// ステートを初期化
	auto stateMachine = GetComponent<StateMachine<Player>>();
	stateMachine->ChangeState(std::make_unique<PlayerStateIdle>(*this));
}

void Player::Update()
{
	Transform& transform = *GetComponent<Transform>();

	//Control();

	// ステートを更新
	GetComponent<StateMachine<Player>>()->Update();

	// physicsの更新
	auto physics = GetComponent<Physics>();
	physics->Update();
	Vector3 velXZ = Vector3(physics->m_vel.x, 0.0f, physics->m_vel.z);
	if (velXZ.SquaredLength() > kMaxMoveSpeed * kMaxMoveSpeed)
	{
		velXZ = velXZ.Normalized() * kMaxMoveSpeed;
	}
	physics->m_vel.x = velXZ.x;
	physics->m_vel.z = velXZ.z;

	// 向きを更新
	float diff = MyLib::GetAngleDiff(m_angle, transform.rot.y);
	transform.rot.y += diff * 0.1f;

	// モデルの行列を更新
	m_pModel->SetTransform(transform);

	// アニメーションを更新
	GetComponent<Animator>()->Update();
}

void Player::Draw()
{
	// モデルを描画
	m_pModel->Draw();
	GetComponent<StateMachine<Player>>()->Draw();	// ステートに描画したい内容があったら描画
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