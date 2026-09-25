#include "PlayerStateMove.h"
#include "../Player.h"
#include "System/PadInput.h"
#include "PlayerStateIdle.h"

PlayerStateMove::PlayerStateMove(Player& owner) :
	State(owner)
{
}

void PlayerStateMove::Enter()
{
	// アニメーションを再生
	m_owner.GetAnimator().Play(m_owner.kAnimNames[static_cast<int>(Player::AnimationID::Jog)]);
}

void PlayerStateMove::Update()
{
	// 入力がなくなったらIdle
	Vector2 stick = PadInput::GetInstance().GetStickInput(PadInput::LR::Left);
	if (stick.SquaredLength() == 0.0f)
	{
		m_owner.GetStateMachine().ChangeState(std::make_unique<PlayerStateIdle>(m_owner));
		return;
	}
}

void PlayerStateMove::Exit()
{
}
