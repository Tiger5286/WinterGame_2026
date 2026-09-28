#include "PlayerStateFall.h"
#include "PlayerStateLand.h"
#include "Components/Animator/Animator.h"
#include "../Player.h"
#include "Components/Physics.h"
#include "Components/State/StateMachine.h"

PlayerStateFall::PlayerStateFall(Player& player) :
	PlayerState(player)
{
}

void PlayerStateFall::Enter()
{
	m_owner.GetComponent<Animator>()->Play(Player::kAnimNames[static_cast<int>(Player::AnimationID::Fall)]);
}

void PlayerStateFall::Update()
{
	if (m_owner.GetComponent<Physics>()->IsGrounded())
	{
		m_owner.GetComponent<Physics>()->SetDrag(Physics::kDefaultDrag);
		m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateLand>(m_owner));
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
