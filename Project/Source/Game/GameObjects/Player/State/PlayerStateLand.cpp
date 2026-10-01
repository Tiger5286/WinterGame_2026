#include "PlayerStateLand.h"
#include "PlayerStateIdle.h"
#include "PlayerStateMove.h"
#include "../Player.h"
#include "Components/Animator/Animator.h"
#include "Components/State/StateMachine.h"
#include "Components/Physics.h"
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
	m_owner.GetComponent<Physics>()->SetDrag(Physics::kDefaultDrag);
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

	auto layer = Animator::Layer::FullBody;
	if (IsAim())
	{
		layer = Animator::Layer::LowerBody;
	}

	if (m_owner.GetComponent<Animator>()->IsEnd(layer))
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
