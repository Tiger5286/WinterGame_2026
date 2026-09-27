#include "ServiceLocator.h"
#include <cassert>

ServiceLocator& ServiceLocator::GetInstance()
{
	static ServiceLocator instance;
	return instance;
}

CollisionManager& ServiceLocator::GetCollisionManager() const
{
	assert(m_pCollisionManager && "ServiceLocator::GetCollisionManager() : CollisionManager‚ªnullptr‚Å‚·B");
	return *m_pCollisionManager;
}
