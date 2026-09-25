#include "PlayerStateIdle.h"
#include "../Player.h"
#include "System/PadInput.h"
#include "PlayerStateMove.h"

PlayerStateIdle::PlayerStateIdle(Player& owner) :
	State(owner)
{
}

void PlayerStateIdle::Enter()
{
	// アニメーションを再生
	m_owner.GetAnimator().Play(m_owner.kAnimNames[static_cast<int>(Player::AnimationID::Idle)]);
}

void PlayerStateIdle::Update()
{
	// スティック入力があったらmove
	Vector2 stick = PadInput::GetInstance().GetStickInput(PadInput::LR::Left);
	if (stick.SquaredLength() > 0.0f)
	{
		m_owner.GetStateMachine().ChangeState(std::make_unique<PlayerStateMove>(m_owner));
		return;
	}
}

void PlayerStateIdle::Exit()
{
}
