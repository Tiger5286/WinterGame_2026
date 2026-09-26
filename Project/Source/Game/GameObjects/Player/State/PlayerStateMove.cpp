#include "PlayerStateMove.h"
#include "../Player.h"
#include "System/PadInput.h"
#include "PlayerStateIdle.h"
#include "Components/Animator/Animator.h"
#include "Components/State/StateMachine.h"
#include "Components/Physics.h"
#include "Utility/Matrix4x4.h"

namespace
{
	constexpr float kJogAccel = 0.5f;
	constexpr float kRunAccel = 1.0f;
}

PlayerStateMove::PlayerStateMove(Player& owner) :
	PlayerState(owner)
{
}

void PlayerStateMove::Enter()
{
}

void PlayerStateMove::Update()
{
	auto physics = m_owner.GetComponent<Physics>();
	auto& input = PadInput::GetInstance();
	Vector2 stick = input.GetStickInput(PadInput::LR::Left);
	// スティック入力があるかどうか
	bool isInputStick = stick.SquaredLength() > 0.0f;
	// 水平の速度が遅いかどうか
	Vector2 velXZ = Vector2(physics->m_vel.x, physics->m_vel.z);
	bool isSlow = velXZ.SquaredLength() < 0.1f;
	
	// スティック入力がない、かつ速度が遅い場合はidleに戻る
	if (!isInputStick && isSlow)
	{
		m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateIdle>(m_owner));
		return;
	}

	// 右スティック押し込みでダッシュ切り替え
	if (input.IsTriggerd(XINPUT_BUTTON_LEFT_THUMB))
	{
		m_isRun = !m_isRun;
	}

	// 移動処理
	// カメラの方向を入力と合成
	Matrix4x4 rot = Matrix4x4::GetRotY(GetCameraAngleY());
	Vector3 stickVec3 = Vector3(stick.x, 0.0f, stick.y);
	stickVec3 *= rot;

	// 状態によって最高速度と加速度を設定
	float maxSpeed = kMaxJogSpeed;
	float accel = kJogAccel;
	if (m_isRun)
	{
		maxSpeed = kMaxRunSpeed;
		accel = kRunAccel;
	}
	physics->SetMaxSpeed(maxSpeed);

	// 入力方向に移動
	physics->m_accel.x = stickVec3.x * accel;
	physics->m_accel.z = stickVec3.z * accel;

	// 入力がある時だけプレイヤーの向きを変える
	if (stickVec3.SquaredLength() > 0.0f)
	{
		float rot = atan2(-stickVec3.z, stickVec3.x) - DX_PI_F / 2;
		SetAngle(rot);
	}
}

void PlayerStateMove::Exit()
{
}
