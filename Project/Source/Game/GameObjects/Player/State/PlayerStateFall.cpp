#include "PlayerStateFall.h"
#include "PlayerStateLand.h"
#include "../Player.h"
#include "Components/Physics.h"
#include "Components/State/StateMachine.h"
#include "System/PadInput.h"
#include "PlayerStateHover.h"

PlayerStateFall::PlayerStateFall(Player& player) :
	PlayerState(player)
{
}

void PlayerStateFall::Enter()
{
	m_owner.GetComponent<Physics>()->SetDrag(1.0f);
}

void PlayerStateFall::Update()
{
	if (m_owner.GetComponent<Physics>()->IsGrounded())
	{
		m_owner.GetComponent<Physics>()->SetDrag(Physics::kDefaultDrag);
		m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateLand>(m_owner));
		return;
	}

	if (IsAim() || PadInput::GetInstance().IsPressed(XINPUT_BUTTON_A))
	{
		m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateHover>(m_owner));
		return;
	}
}

void PlayerStateFall::Exit()
{
}

PlayerStateFall::ID PlayerStateFall::GetID() const
{
	return ID::Fall;
}
