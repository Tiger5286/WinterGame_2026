#include "WalkerState.h"
#include "../Walker.h"

WalkerState::WalkerState(Walker& walker) :
	State(walker)
{
}

WalkerState::~WalkerState()
{
}

std::shared_ptr<Player> WalkerState::GetPlayer() const
{
	return m_owner.m_pPlayer.lock();
}

int WalkerState::GetAttackCooltime() const
{
	return m_owner.m_attackCooltime;
}

void WalkerState::SetAttackCooltime(int coolTime)
{
	m_owner.m_attackCooltime = coolTime;
}

void WalkerState::Die()
{
	m_owner.m_isDead = true;
}

void WalkerState::RotToPlayer()
{
	m_owner.RotToPlayer();
}