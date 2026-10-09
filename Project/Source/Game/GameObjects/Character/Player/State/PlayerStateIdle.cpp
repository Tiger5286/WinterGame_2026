#include "PlayerStateIdle.h"
#include "../Player.h"
#include "System/PadInput.h"
#include "PlayerStateMove.h"
#include "PlayerStateJump.h"
#include "Components/Animator/Animator.h"
#include "Components/State/StateMachine.h"
#include "PlayerStateFall.h"
#include "Components/Physics.h"

PlayerStateIdle::PlayerStateIdle(Player& owner) :
	PlayerState(owner)
{
}

void PlayerStateIdle::Enter()
{
	SetIsRun(false);
	m_owner.GetComponent<Physics>()->SetDrag(Physics::kDefaultDrag);
}

void PlayerStateIdle::Update()
{
	auto& pad = PadInput::GetInstance();
	auto stateMachine = m_owner.GetComponent<StateMachine<Player>>();
	// スティック入力があったらmove
	Vector2 stick = pad.GetStickInput(PadInput::LR::Left);
	if (stick.SquaredLength() > 0.0f)
	{
		stateMachine->ChangeState(std::make_unique<PlayerStateMove>(m_owner));
		return;
	}
	// ジャンプボタンを押したらジャンプ
	if (pad.IsTriggerd(XINPUT_BUTTON_A) && !IsAim())
	{
		stateMachine->ChangeState(std::make_unique<PlayerStateJump>(m_owner));
		return;
	}
	// 接地していない場合は落下
	if (!m_owner.GetComponent<Physics>()->IsGrounded())
	{
		stateMachine->ChangeState(std::make_unique<PlayerStateFall>(m_owner));
		return;
	}
}

void PlayerStateIdle::Exit()
{
}