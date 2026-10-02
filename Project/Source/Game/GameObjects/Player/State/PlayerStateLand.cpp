#include "PlayerStateLand.h"
#include "PlayerStateIdle.h"
#include "PlayerStateMove.h"
#include "PlayerStateJump.h"
#include "../Player.h"
#include "Components/Animator/Animator.h"
#include "Components/State/StateMachine.h"
#include "Components/Physics.h"
#include "System/PadInput.h"

namespace
{
	constexpr int kMinFrame = 10;
}

PlayerStateLand::PlayerStateLand(Player& player) :
	PlayerState(player)
{
}

void PlayerStateLand::Enter()
{
	m_owner.GetComponent<Physics>()->SetDrag(Physics::kDefaultDrag);
}

void PlayerStateLand::Update()
{
	m_frame++;

	// 移動処理
	float accel = kJogAccel;
	float maxSpeed = kMaxJogSpeed;
	if (IsRun())
	{
		accel = kRunAccel;
		maxSpeed = kMaxRunSpeed;
	}
	UpdateMove(accel, maxSpeed);

	const bool isInputStick = PadInput::GetInstance().GetStickInput(PadInput::LR::Left).SquaredLength() > 0.0f;

	// エイムしていたらLandステートを終わる
	if (IsAim())
	{
		if (isInputStick)
		{
			m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateMove>(m_owner));
			return;
		}
		else
		{
			m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateIdle>(m_owner));
			return;
		}
	}

	// ジャンプボタンを押したらジャンプする
	if (PadInput::GetInstance().IsTriggerd(XINPUT_BUTTON_A))
	{
		m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateJump>(m_owner));
		return;
	}

	// 一定フレーム経過後にスティック入力があったらMoveステートに遷移する
	if (m_frame > kMinFrame && isInputStick)
	{
		m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateMove>(m_owner));
		return;
	}

	// 着地アニメーションが終わったらIdleステートに遷移する
	if (m_owner.GetComponent<Animator>()->IsEnd())
	{
		m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateIdle>(m_owner));
		return;
	}
}

void PlayerStateLand::Exit()
{
}

PlayerStateLand::ID PlayerStateLand::GetID() const
{
	return ID::Land;
}
