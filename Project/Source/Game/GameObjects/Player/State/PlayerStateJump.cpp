#include "PlayerStateJump.h"
#include "../Player.h"
#include "Components/Physics.h"
#include "Components/Animator/Animator.h"
#include "PlayerStateIdle.h"
#include "Components/State/StateMachine.h"

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
	m_owner.GetComponent<Animator>()->Play(Player::kAnimNames[static_cast<int>(Player::AnimationID::Jump)]);
}

void PlayerStateJump::Update()
{
	if (m_owner.GetComponent<Animator>()->IsEnd())
	{
		m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateIdle>(m_owner));
	}
}

void PlayerStateJump::Exit()
{
}

PlayerStateJump::ID PlayerStateJump::GetID() const
{
	return ID::Jump;
}
