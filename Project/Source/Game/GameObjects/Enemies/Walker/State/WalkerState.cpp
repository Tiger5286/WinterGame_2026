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
