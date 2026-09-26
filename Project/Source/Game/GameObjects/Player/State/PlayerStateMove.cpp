#include "PlayerStateMove.h"
#include "../Player.h"
#include "System/PadInput.h"
#include "PlayerStateIdle.h"
#include "Components/Animator/Animator.h"
#include "Components/State/StateMachine.h"

PlayerStateMove::PlayerStateMove(Player& owner) :
	PlayerState(owner)
{
}

void PlayerStateMove::Enter()
{
}

void PlayerStateMove::Update()
{
	// “ü—Í‚ª‚È‚­‚È‚Á‚½‚çIdle
	Vector2 stick = PadInput::GetInstance().GetStickInput(PadInput::LR::Left);
	if (stick.SquaredLength() == 0.0f)
	{
		m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateIdle>(m_owner));
		return;
	}
}

void PlayerStateMove::Exit()
{
}
