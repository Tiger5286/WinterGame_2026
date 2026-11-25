#include "EnemyBase.h"

namespace
{
	// 弱点を撃たれた時のダメージ倍率
	constexpr float kWeakPointDamageRate = 1.5f;
	// ハッキングされてないときのダメージ減衰率
	constexpr float kNotHackedDamageRate = 0.1f;
	// クリティカルノード発動時のダメージ倍率
	constexpr float kCriticalNodeRate = 3.0f;
}

EnemyBase::EnemyBase(const SpawnContext::Info& info) :
	m_pPlayer(info.pPlayer),
	m_pGameObjectManager(info.pGameObjectManager)
{
	m_tag = GameObject::Tag::Enemy;
}

EnemyBase::~EnemyBase()
{
}

void EnemyBase::OnDamage(int damage)
{
	// 死亡中はダメージを受けない
	if (IsDying()) return;

	m_hp -= damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		Death();
	}
}

void EnemyBase::OnHacked(Hackable::HackedData data)
{
	int damage = data.damage;
	for (const auto& special : data.specials)
	{
		if (special == Hackable::SpecialNode::Critical)
		{
			damage *= kCriticalNodeRate;
			break;
		}
	}
	OnDamage(damage);
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
			damage = info.damage * kWeakPointDamageRate;
		}
		else
		{
			damage = info.damage;
		}
	}
	else
	{
		damage = info.damage * kNotHackedDamageRate;
	}

	OnDamage(damage);
}

void EnemyBase::DropSpecialNode()
{
	// TODO:特殊ノードを落とす
}
