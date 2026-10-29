#include "PlayerStateHit.h"
#include "PlayerStateIdle.h"
#include "Components/Animator/Animator.h"
#include "Components/State/StateMachine.h"
#include "../Player.h"

PlayerStateHit::PlayerStateHit(Player& player) :
	PlayerState(player)
{
}

PlayerStateHit::~PlayerStateHit()
{
}

void PlayerStateHit::Enter()
{
}

void PlayerStateHit::Update()
{
	auto animator = m_owner.GetComponent<Animator>();
	if (animator->IsEnd())
	{
		auto stateMachine = m_owner.GetComponent<StateMachine<Player>>();
		stateMachine->ChangeState(std::make_unique<PlayerStateIdle>(m_owner));
		return;
	}
}

void PlayerStateHit::Exit()
{
}