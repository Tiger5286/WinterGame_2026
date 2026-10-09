#include "WalkerStateOrbit.h"
#include "Game/GameObjects/Player/Player.h"
#include "../Walker.h"
#include "Components/Transform.h"
#include "Components/Physics.h"
#include "Components/State/StateMachine.h"
#include "WalkerStateApproach.h"

namespace
{
	constexpr float kMaxMoveSpeed = 1.0f;
}

WalkerStateOrbit::WalkerStateOrbit(Walker& walker) :
	WalkerState(walker)
{
}

WalkerStateOrbit::~WalkerStateOrbit()
{
}

void WalkerStateOrbit::Enter()
{
	m_owner.GetComponent<Physics>()->SetMaxSpeed(kMaxMoveSpeed);
}

void WalkerStateOrbit::Update()
{
	Vector3 playerPosXZ = GetPlayer()->GetComponent<Transform>()->pos;
	playerPosXZ.y = 0.0f;
	Vector3 thisPosXZ = m_owner.GetComponent<Transform>()->pos;
	thisPosXZ.y = 0.0f;

	Vector3 thisToPlayerXZ = playerPosXZ - thisPosXZ;

	if (thisToPlayerXZ.SquaredLength() > kMinDistance * kMinDistance)
	{
		m_owner.GetComponent<StateMachine<Walker>>()->ChangeState(std::make_unique<WalkerStateApproach>(m_owner));
		return;
	}

	Vector3 right = thisToPlayerXZ.Cross(Vector3::Up()).Normalized();

	m_owner.GetComponent<Physics>()->m_accel = right * kAccel;
}

void WalkerStateOrbit::Exit()
{
	m_owner.GetComponent<Physics>()->SetMaxSpeed(Physics::kDefaultMaxSpeed);
}
