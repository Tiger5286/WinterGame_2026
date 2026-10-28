#include "Attack.h"
#include "System/ServiceLocator.h"
#include "Game/CollisionManager.h"
#include "Components/Collider/SphereCollider.h"
#include "Components/Transform.h"

namespace
{
#ifdef _DEBUG
	constexpr unsigned int kColColor = 0x00ff00;
#endif
}

Attack::Attack()
{
}

Attack::~Attack()
{
}

void Attack::Init()
{
	ServiceLocator::GetInstance().GetCollisionManager().Register(shared_from_this());

	AddComponent<SphereCollider>(*GetComponent<Transform>(),m_data.colRadius);
}

void Attack::Update()
{
}

void Attack::Draw()
{
#ifdef _DEBUG
	GetComponent<SphereCollider>()->Draw(kColColor);
#endif
}

void Attack::SetData(const Data& data)
{
	m_data = data;
}

void Attack::OnCollision(GameObject& other)
{
	// 対象のタグ以外には当たらない
	if (m_data.hitObjTag != Tag::None && other.GetTag() != m_data.hitObjTag) return;
	// 一度当たったオブジェクトには当たらない
	for (const auto& id : m_hitObjectIds)
	{
		if (id == other.GetId()) return;
	}
	// 当たったオブジェクトのIDを覚える
	m_hitObjectIds.push_back(other.GetId());
	// 攻撃に当たった関数を呼ぶ
	other.OnHitAttack(m_data.info);
}
