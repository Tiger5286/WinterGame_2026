#include "WalkerStateApproach.h"
#include "Utility/Vector3.h"
#include "Game/GameObjects/Character/Player/Player.h"
#include "Components/Transform.h"
#include "Game/GameObjects/Character/Enemies/Walker/Walker.h"
#include "Components/Physics.h"
#include "WalkerStateOrbit.h"
#include "Components/State/StateMachine.h"

namespace
{
	constexpr float kMaxMoveSpeed = 2.0f;
}

WalkerStateApproach::WalkerStateApproach(Walker& walker) :
	WalkerState(walker)
{
}

WalkerStateApproach::~WalkerStateApproach()
{
}

void WalkerStateApproach::Enter()
{
	m_owner.GetComponent<Physics>()->SetMaxSpeed(kMaxMoveSpeed);
}

void WalkerStateApproach::Update()
{
	Vector3 playerPosXZ = GetPlayer()->GetComponent<Transform>()->pos;
	playerPosXZ.y = 0.0f;
	Vector3 thisPosXZ = m_owner.GetComponent<Transform>()->pos;
	thisPosXZ.y = 0.0f;

	Vector3 thisToPlayer = (playerPosXZ - thisPosXZ);

	if (thisToPlayer.SquaredLength() < kMinDistance * kMinDistance)
	{
		m_owner.GetComponent<StateMachine<Walker>>()->ChangeState(std::make_unique<WalkerStateOrbit>(m_owner));
		return;
	}

	thisToPlayer.Normalize();
	thisToPlayer *= kAccel;
	m_owner.GetComponent<Physics>()->m_accel = thisToPlayer;
}

void WalkerStateApproach::Exit()
{
	m_owner.GetComponent<Physics>()->SetMaxSpeed(Physics::kDefaultMaxSpeed);
}
