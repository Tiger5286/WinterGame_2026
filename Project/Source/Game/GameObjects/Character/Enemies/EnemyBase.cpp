#include "EnemyBase.h"

EnemyBase::EnemyBase(std::shared_ptr<Player> pPlayer) :
	m_pPlayer(pPlayer)
{
	m_tag = GameObject::Tag::Enemy;
}

EnemyBase::~EnemyBase()
{
}

void EnemyBase::OnDamage(int damage)
{
	m_hp -= damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		Death();
	}
}

void EnemyBase::OnWasShot(const CollisionManager::ShotInfo& info)
{
	// 死亡中は撃たれても何もしない(死亡ステートに入り直さない)
	if (IsDying()) return;

	int damage = 0;
	if (GetComponent<Hackable>()->IsHacked())
	{
		if (info.isWeakPoint)
		{
			damage = info.damage * 1.5f;
		}
		else
		{
			damage = info.damage;
		}
	}
	else
	{
		damage = info.damage / 10;
	}

	OnDamage(damage);
}