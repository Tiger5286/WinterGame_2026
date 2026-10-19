#include "PlayerStateDodge.h"
#include "PlayerStateMove.h"
#include "Components/Physics.h"
#include "../Player.h"
#include "Components/State/StateMachine.h"
#include "System/PadInput.h"
#include "Utility/Matrix4x4.h"

namespace
{
	constexpr float kMaxSpeed = 30.0f;
	constexpr float kMaxAccel = 10.0f;
	constexpr int kDodgeFrame = 20;
}

PlayerStateDodge::PlayerStateDodge(Player& player) :
	PlayerState(player)
{
}

PlayerStateDodge::~PlayerStateDodge()
{
}

void PlayerStateDodge::Enter()
{
	const float angle = GetAngle();
	Vector3 dir = Vector3(-sin(angle),0.0f,-cos(angle));
	m_dir = dir.Normalized();

	auto physics = m_owner.GetComponent<Physics>();
	physics->SetMaxSpeed(kMaxSpeed);
}

void PlayerStateDodge::Update()
{
	m_frame++;
	auto physics = m_owner.GetComponent<Physics>();
	if (m_frame < 5)
	{
		physics->m_accel = m_dir * m_frame * 3;
	}
	else
	{
		physics->m_accel = Vector3::Zero();
	}

	if (m_frame > kDodgeFrame)
	{
		m_owner.GetComponent<StateMachine<Player>>()->ChangeState(std::make_unique<PlayerStateMove>(m_owner));
		return;
	}
}

void PlayerStateDodge::Exit()
{
}
