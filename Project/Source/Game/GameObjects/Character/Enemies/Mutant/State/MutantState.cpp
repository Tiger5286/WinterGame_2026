#include "MutantState.h"
#include "../Mutant.h"
#include "Game/GameObjects/Character/Player/Player.h"
#include "Components/Transform.h"

MutantState::MutantState(Mutant& mutant) :
	State(mutant)
{
}

MutantState::~MutantState()
{
}

std::shared_ptr<Player> MutantState::GetPlayer() const
{
	return m_owner.m_pPlayer.lock();
}

int MutantState::GetAttackCooltime() const
{
	return m_owner.m_attackCooltime;
}

void MutantState::SetAttackCooltime(int coolTime)
{
	m_owner.m_attackCooltime = coolTime;
}

void MutantState::Die()
{
	m_owner.m_isDead = true;
}

void MutantState::RotToPlayer()
{
	m_owner.RotToPlayer();
}

Vector3 MutantState::GetToPlayerXZ() const
{
	Vector3 playerPosXZ = GetPlayer()->GetComponent<Transform>()->pos;
	playerPosXZ.y = 0.0f;
	Vector3 thisPosXZ = m_owner.GetComponent<Transform>()->pos;
	thisPosXZ.y = 0.0f;
	return playerPosXZ - thisPosXZ;
}
