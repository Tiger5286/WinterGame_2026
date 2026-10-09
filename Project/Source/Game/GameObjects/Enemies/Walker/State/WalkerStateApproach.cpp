#include "WalkerStateApproach.h"
#include "Utility/Vector3.h"
#include "Game/GameObjects/Player/Player.h"
#include "Components/Transform.h"
#include "Game/GameObjects/Enemies/Walker/Walker.h"
#include "Components/Physics.h"

namespace
{
	constexpr float kMaxMoveSpeed = 2.0f;
	constexpr float kAccel = 0.15f;
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

	Vector3 thisToPlayer = (playerPosXZ - thisPosXZ).Normalized();
	thisToPlayer *= kAccel;
	m_owner.GetComponent<Physics>()->m_accel = thisToPlayer;
}

void WalkerStateApproach::Exit()
{
}
