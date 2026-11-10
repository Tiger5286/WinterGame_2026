#include "Missile.h"
#include "DxLib.h"
#include "Components/Transform.h"
#include "Utility/MyLib.h"
#include "Attack.h"

namespace
{
	constexpr int kImpactFrame = 120;
	constexpr int kImpactEndFrame = 125;
	constexpr int kMaxFrame = 150;

	constexpr float kRadius = 200.0f;
}

Missile::Missile(const Vector3& pos,const int damage) :
	m_damage(damage)
{
	GetComponent<Transform>()->pos = pos;
}

Missile::~Missile()
{
}

void Missile::Init()
{
}

void Missile::Update()
{
	m_frameCount++;
	if (m_frameCount == kImpactFrame)
	{
		// 爆発処理
		m_pAtk = std::make_shared<Attack>();
		CollisionManager::AttackInfo info = {
			.damage = m_damage
		};
		Attack::Data data = {
			.info = info,
			.colRadius = kRadius,
			.hitObjTag = GameObject::Tag::Player
		};
		m_pAtk->SetData(data);
		m_pAtk->Init();
		m_pAtk->GetComponent<Transform>()->pos = GetComponent<Transform>()->pos;
	}
	if (m_frameCount == kImpactEndFrame)
	{
		m_pAtk = nullptr;
	}
	if (m_frameCount >= kMaxFrame)
	{
		m_isDead = true;
	}
}

void Missile::Draw()
{
	MyLib::DrawCircle3D(GetComponent<Transform>()->pos, kRadius, 32, 0xff0000);
	if (m_pAtk)
	{
		m_pAtk->Draw();
	}
}