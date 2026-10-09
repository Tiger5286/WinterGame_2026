#include "EnemyBase.h"

EnemyBase::EnemyBase(std::shared_ptr<Player> pPlayer) :
	m_pPlayer(pPlayer)
{
	m_tag = GameObject::Tag::Enemy;
}

EnemyBase::~EnemyBase()
{
}