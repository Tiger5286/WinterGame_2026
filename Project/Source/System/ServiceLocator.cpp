#include "ServiceLocator.h"
#include <cassert>

ServiceLocator& ServiceLocator::GetInstance()
{
	static ServiceLocator instance;
	return instance;
}

CollisionManager& ServiceLocator::GetCollisionManager() const
{
	assert(m_pCollisionManager && "ServiceLocator::GetCollisionManager() : CollisionManagerがnullptrです。");
	return *m_pCollisionManager;
}
