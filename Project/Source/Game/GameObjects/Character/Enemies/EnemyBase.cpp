#include "EnemyBase.h"
#include "Game/GameObjectManager.h"
#include "Game/GameObjects/SpecialNodeItem.h"
#include "Components/Transform.h"

namespace
{
	// 弱点を撃たれた時のダメージ倍率
	constexpr float kWeakPointDamageRate = 1.5f;
	// ハッキングされてないときのダメージ減衰率
	constexpr float kNotHackedDamageRate = 0.1f;
	// クリティカルノード発動時のダメージ倍率
	constexpr float kCriticalNodeRate = 3.0f;

	// 死んだとき特殊ノードを落とす確率(%)
	constexpr int kDropSpecialNodeProbability = 40;
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
		if (special == SpecialNodes::Type::Critical)
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
	auto goManager = m_pGameObjectManager.lock();
	if (!goManager) return;

	int rand = GetRand(99);
	if (rand < kDropSpecialNodeProbability)
	{
		Vector3 pos = GetComponent<Transform>()->pos;
		goManager->Add(std::make_shared<SpecialNodeItem>(pos, SpecialNodes::GetSpecialNodeTypeRandom(), m_pPlayer.lock()));
	}
}
