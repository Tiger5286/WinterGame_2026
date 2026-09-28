#include "PlayerStateMove.h"
#include "../Player.h"
#include "System/PadInput.h"
#include "PlayerStateIdle.h"
#include "PlayerStateJump.h"
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
		physics->m_accel.x = 0.0f;
		physics->m_accel.z = 0.0f;
		m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateIdle>(m_owner));
		return;
	}

	// ジャンプボタンを押したらジャンプ
	if (input.IsTriggerd(XINPUT_BUTTON_A))
	{
		physics->m_accel.x = 0.0f;
		physics->m_accel.z = 0.0f;
		physics->SetDrag(1.0f);
		m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateJump>(m_owner));
		return;
	}

	// 右スティック押し込みでダッシュ切り替え
	if (input.IsTriggerd(XINPUT_BUTTON_LEFT_THUMB))
	{
		SetIsRun(!IsRun());
	}

	// 移動処理
	// 状態によって最高速度と加速度を設定
	float maxSpeed = kMaxJogSpeed;
	float accel = kJogAccel;
	if (IsRun())
	{
		maxSpeed = kMaxRunSpeed;
		accel = kRunAccel;
	}
	UpdateMove(accel, maxSpeed);
	//physics->SetMaxSpeed(maxSpeed);
}

void PlayerStateMove::Exit()
{
}
