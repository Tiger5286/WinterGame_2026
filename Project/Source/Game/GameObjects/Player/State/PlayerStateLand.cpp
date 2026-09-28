#include "PlayerStateLand.h"
#include "PlayerStateIdle.h"
#include "PlayerStateMove.h"
#include "../Player.h"
#include "Components/Animator/Animator.h"
#include "Components/State/StateMachine.h"
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
	m_owner.GetComponent<Animator>()->Play(Player::kAnimNames[static_cast<int>(Player::AnimationID::Land)]);
}

void PlayerStateLand::Update()
{
	m_frame++;

	float accel = kJogAccel;
	float maxSpeed = kMaxJogSpeed;
	if (IsRun())
	{
		accel = kRunAccel;
		maxSpeed = kMaxRunSpeed;
	}
	UpdateMove(accel, maxSpeed);

	if (m_frame > kMinFrame &&
		PadInput::GetInstance().GetStickInput(PadInput::LR::Left).SquaredLength() > 0.0f)
	{
		m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateMove>(m_owner));
		return;
	}

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
