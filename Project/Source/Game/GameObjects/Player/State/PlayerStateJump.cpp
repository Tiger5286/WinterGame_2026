#include "PlayerStateJump.h"
#include "../Player.h"
#include "Components/Physics.h"
#include "PlayerStateFall.h"
#include "Components/State/StateMachine.h"
#include "PlayerStateHover.h"
#include "System/PadInput.h"
#include "Components/Physics.h"

namespace
{
	constexpr float kJumpPower = 10.0f;
}

PlayerStateJump::PlayerStateJump(Player& player) :
	PlayerState(player)
{
}

void PlayerStateJump::Enter()
{
	m_owner.GetComponent<Physics>()->m_vel.y = kJumpPower;
	m_owner.GetComponent<Physics>()->SetDrag(1.0f);
}

void PlayerStateJump::Update()
{
	auto physics = m_owner.GetComponent<Physics>();

	// 落ち始めたら落下ステートに遷移
	if (physics->m_vel.y < 0.0f)
	{
		m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateFall>(m_owner));
		return;
	}

	// エイムしたらホバーに遷移
	if (IsAim())
	{
		m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateHover>(m_owner));
		return;
	}

	// ジャンプボタンを押してもホバーに遷移
	if (PadInput::GetInstance().IsPressed(XINPUT_BUTTON_A))
	{
		if (m_owner.GetComponent<Physics>()->m_vel.y < 0.5f)
		{
			m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateHover>(m_owner));
			return;
		}
	}
}

void PlayerStateJump::Exit()
{
}

PlayerStateJump::ID PlayerStateJump::GetID() const
{
	return ID::Jump;
}
